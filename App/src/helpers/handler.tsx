import CurrentSettings from "./settings";
import Socket, { SocketClass } from "./socket";
import toastController from "./toastController";
import { EChatState, EContactState, EMessageSender, EMessageState, IContact, IMessage } from "./types";
import {Crypto} from "capacitor-crypto";
import * as sanitizeHtml from 'sanitize-html';

// handle all message.
export class Handler {
    constructor(sock : SocketClass) {
        sock.on("chatRequest", this.handleContactRequest);
        sock.on("keyExchange", this.handleKeyExchange);
        sock.on("finishKeyExchange", this.handleFinishKeyExchange);
        sock.on("newMessage", this.handleNewMessage);
        sock.on("messageState", this.handleMessageState);
        
        this.sock = sock;
    }

    private async handleMessageState(data : any) {
        if(!CurrentSettings.doesContactExist(data.username))
            return;

        if(CurrentSettings.getContactState(data.username) !== EContactState.ACCEPTED)
            return;
    
        let Chat = CurrentSettings.getChat(data.username);

        if(Chat.getState() !== EChatState.READY)
            return;

        Chat.setMessageState(data.messageId, data.state);
    }
    private async handleNewMessage(data : any) {
        if(!CurrentSettings.doesContactExist(data.username))
            return;

        if(CurrentSettings.getContactState(data.username) !== EContactState.ACCEPTED)
            return;
    
        let Chat = CurrentSettings.getChat(data.username);

        // this needs to be managed in the future.
        if(Chat.getState() !== EChatState.READY)
            return;

        let messageId = data.id;
        let messageEncrypted = data.encryptedData;
        let messageTime = data.time;
        let messageTag = data.eTag;
        let messageIv = data.iv;

        let messageDecrypted = await Crypto.decrypt({
            key: Chat.getSharedSecret(),
            encryptedData: messageEncrypted,
            tag: messageTag,
            iv: messageIv
        }).catch((err) => {
            console.error(err);
        });

        const newMessage : IMessage = {
            id: messageId,
            contents: typeof(messageDecrypted) === "undefined" ? "--- FAILED TO DECRYPT ---" : messageDecrypted.unencryptedData,
            time: messageTime,
            state: EMessageState.SENT,
            who: EMessageSender.RECEIVER
        };

        Socket.relay(data.username, "messageState", {
            username: Socket.getSocketUsername(),
            messageId: messageId,
            state: EMessageState.DELIVERED
        });

        if(window.location.pathname !== `/chat/${data.username}`) {
            toastController.sendToast({
                message: `<strong>${sanitizeHtml.default(data.username,{allowedTags: [],allowedAttributes: {}})}</strong><br/>${sanitizeHtml.default(newMessage.contents,{allowedTags: [],allowedAttributes: {}})}`,
                duration: 1000,
                color: "dark",
                position: "top"
            });
        }

        Chat.addMessage(messageId, newMessage);
    }
    private async handleKeyExchange(data : any) {
        if(!CurrentSettings.doesContactExist(data.username))
            return;

        if(CurrentSettings.getContactState(data.username) !== EContactState.ACCEPTED)
            return;
        
        let Chat = CurrentSettings.getChat(data.username);

        // this needs to be managed in the future.
        if(Chat.getState() !== EChatState.KEYEXCHANGE)
            return;

        let publicKey = data.publicKey;

        if(Chat.getKeys().privateKey.length <= 0) {
            let newKeys = await Crypto.generateKeyPair();
            Chat.setKeys(newKeys);
        }

        let salt = await Crypto.generateRandomBytes({
            count: 16
        });

        let sharedSecret = await Crypto.generateSharedSecret({
            privateKey: Chat.getKeys().privateKey,
            publicKey: publicKey,
            salt: salt.bytesBase64
        });

        Chat.setSharedSecret(sharedSecret.sharedSecret);

        Chat.setState(EChatState.READY);

        Socket.relay(Chat.getUsername(), "finishKeyExchange", {
            username: Socket.getSocketUsername(),
            publicKey: Chat.getKeys().publicKey,
            salt: salt.bytesBase64
        });
    }

    private async handleFinishKeyExchange(data : any) {
        if(!CurrentSettings.doesContactExist(data.username))
            return;

        if(CurrentSettings.getContactState(data.username) !== EContactState.ACCEPTED)
            return;
    
        let Chat = CurrentSettings.getChat(data.username);

        // this needs to be managed in the future.
        if(Chat.getState() !== EChatState.KEYEXCHANGE)
            return;

        let publicKey = data.publicKey;

        if(Chat.getKeys().privateKey.length <= 0) {
            console.error("no keys in finish stage, redo.");
            return;
        }

        let salt = data.salt;

        let sharedSecret = await Crypto.generateSharedSecret({
            privateKey: Chat.getKeys().privateKey,
            publicKey: publicKey,
            salt: salt
        });

        Chat.setSharedSecret(sharedSecret.sharedSecret);

        Chat.setState(EChatState.READY);
    }
    private handleContactRequest(data : any) {
        /*{
            "username": Socket.getSocketUsername(),
            "state": EContactState.SENT
        }*/

        let contact = data as IContact;
        switch(contact.contactState) {
            case EContactState.SENT:
                let newContact = contact;
                newContact.contactState = EContactState.PENDING;
                CurrentSettings.addContact(newContact);
                toastController.sendToast({
                    message: "You've got a new chat request from "+ newContact.username,
                    duration: 1000
                });
                break;
            case EContactState.DENIED:
                if(CurrentSettings.editContactState(contact.username, EContactState.DENIED)) {
                    toastController.sendToast({
                        message: `${contact.username} denied your request.`,
                        duration: 1000,
                        color: "danger"
                    });
                }
                // do nothing if this fails, could be a replayed message
                break;
            case EContactState.ACCEPTED:
                if(CurrentSettings.editContactState(contact.username, EContactState.ACCEPTED)) {
                    toastController.sendToast({
                        message: `${contact.username} accepted your request.`,
                        duration: 1000,
                        color: "success"
                    });
                }
                break;
            case EContactState.REJECTED:
                if(CurrentSettings.editContactState(contact.username, EContactState.DENIED)) {
                    toastController.sendToast({
                        message: `${contact.username} does not exist.`,
                        duration: 1000,
                        color: "danger"
                    })
                }
                break;
        }
    }

    private sock : SocketClass;
};