import Chat from "./classes/chat";
import Socket from "./socket";
import Store from "./store";
import { EChatState, EContactState, IChat, IContact, ISettings } from "./types";

class Settings
{
    constructor() {
        this.settingsObject = this.getDefaultObject();
    }

    public getSettings(addr : string) : Promise<void> {
        return new Promise((resolve, reject) => {
            this.currentAddr = addr;
            Store.get(this.currentAddr).then((data) => {
                if(data === null) {
                    this.settingsObject = this.getDefaultObject();
                    this.storeSettings();
                } else {
                    this.settingsObject = data;
                }
                resolve();
            }).catch((err) => {
                console.error("[SETTINGS] Failed to get current settings: "+err);
                reject();
            });
        });
    }

    public addContact(contactInfo : IContact, ourUsername? : string) : void {
        let username = typeof(ourUsername) === "undefined" ? Socket.getSocketUsername() : ourUsername;
        this.initUserData(username);
        this.settingsObject.userData[username].contacts[contactInfo.username] = contactInfo;
        if(this.contactsChangedCb !== null) {
            this.contactsChangedCb();
        }

        if(this.pendingRequestsCb !== null) {
            this.pendingRequestsCb();
        }

        this.storeSettings();
    }
    public doesContactExist(contactName : string, ourUsername ? : string) {
        let username = typeof(ourUsername) === "undefined" ? Socket.getSocketUsername() : ourUsername;
        this.initUserData(username);

        return typeof(this.settingsObject.userData[username].contacts[contactName]) !== "undefined";
    }
    public getContactState(contactName : string, ourUsername ? : string) : EContactState {
        let username = typeof(ourUsername) === "undefined" ? Socket.getSocketUsername() : ourUsername;
        this.initUserData(username);

        return this.settingsObject.userData[username].contacts[contactName].contactState;
    }
    public editContactState(contactName : string, newState : EContactState, ourUsername? : string) : boolean {
        let username = typeof(ourUsername) === "undefined" ? Socket.getSocketUsername() : ourUsername;
        this.initUserData(username);

        // use add contact to add a new one not edit state
        if(!this.doesContactExist(contactName, username))
            return false;

        if(newState == EContactState.DENIED) {
            delete this.settingsObject.userData[username].contacts[contactName];
        } else {
            this.settingsObject.userData[username].contacts[contactName].contactState = newState;
        }

        if(this.contactsChangedCb !== null) {
            this.contactsChangedCb();
        }

        if(this.pendingRequestsCb !== null) {
            this.pendingRequestsCb();
        }

        this.storeSettings();

        return true;
    };

    public getPendingContacts(ourUsername? : string) : number {
        if(Socket.getSocketUsername() === "")
            return 0;
        let username = typeof(ourUsername) === "undefined" ? Socket.getSocketUsername() : ourUsername;
        this.initUserData(username);
        let pendingContacts = Object.values(this.settingsObject.userData[username].contacts).filter((val) => val.contactState === EContactState.PENDING);
        return pendingContacts.length;
    }
    public getContacts(ourUsername? : string) : IContact[] {
        if(Socket.getSocketUsername() === "")
            return [];
        let username = typeof(ourUsername) === "undefined" ? Socket.getSocketUsername() : ourUsername;
        this.initUserData(username);
        return Object.values(this.settingsObject.userData[username].contacts);
    }

    public setPendingContactsCallback(pcCb : (() => void))
    {
        this.pendingRequestsCb = pcCb;
    }

    public setContactCallback(contactCb : (() => void)) {
        this.contactsChangedCb = contactCb;
    }

    public setChatCallback(chatCb : (() => void)) {
        this.chatChangedCb = chatCb;
    }

    public getChat(chatUsername : string, ourUsername? : string) : Chat {
        let username = typeof(ourUsername) === "undefined" ? Socket.getSocketUsername() : ourUsername;
        this.initUserData(username);
        this.initChatData(chatUsername);

        return new Chat(this.settingsObject.userData[username].chats[chatUsername]);
    };

    public deleteChat(chatUsername : string, ourUsername? : string) : void {
        let username = typeof(ourUsername) === "undefined" ? Socket.getSocketUsername() : ourUsername;
        this.initUserData(username);
        this.initChatData(chatUsername);

        delete this.settingsObject.userData[username].chats[chatUsername];

        if(this.chatChangedCb !== null) {
            this.chatChangedCb();
        }

        this.storeSettings();
    };

    public getChatData(chatUsername : string, ourUsername? : string) : IChat {
        let username = typeof(ourUsername) === "undefined" ? Socket.getSocketUsername() : ourUsername;
        this.initUserData(username);
        this.initChatData(chatUsername);

        return this.settingsObject.userData[username].chats[chatUsername];
    };

    public updateChat(chatUsername : string, newData : IChat, ourUsername? : string) {
        let username = typeof(ourUsername) === "undefined" ? Socket.getSocketUsername() : ourUsername;
        this.initUserData(username);
        this.initChatData(chatUsername);

        this.settingsObject.userData[username].chats[chatUsername] = newData;

        if(this.chatChangedCb !== null) {
            this.chatChangedCb();
        }

        this.storeSettings();
    }

    public getChats(ourUsername? : string) : IChat[] {
        let username = typeof(ourUsername) === "undefined" ? Socket.getSocketUsername() : ourUsername;
        this.initUserData(username);

        return Object.values(this.settingsObject.userData[username].chats);
    };


    
    public getNotificationsValue(ourUsername? : string) : boolean {
        let username = typeof(ourUsername) === "undefined" ? Socket.getSocketUsername() : ourUsername;
        this.initUserData(username);

        return this.settingsObject.userData[username].notifications;
    }

    public setNotificationsValue(newValue : boolean, ourUsername? : string) : void {
        let username = typeof(ourUsername) === "undefined" ? Socket.getSocketUsername() : ourUsername;
        this.initUserData(username);
        this.settingsObject.userData[username].notifications = newValue;

        this.storeSettings();
    }

    private initChatData(chatUsername : string, ourUsername? : string) : void {
        let username = typeof(ourUsername) === "undefined" ? Socket.getSocketUsername() : ourUsername;
        if(typeof(this.settingsObject.userData[username].chats[chatUsername]) === "undefined") {
            this.settingsObject.userData[username].chats[chatUsername] = {
                username: chatUsername,
                state: EChatState.KEYEXCHANGE,
                messages: {}
            };
            this.storeSettings();
        }
    }

    public initUserData(username : string, ourUsername? : string) : void {
        if(typeof(this.settingsObject.userData[username]) === "undefined") {
            this.settingsObject.userData[username] = {
                chats: {},
                contacts: {},
                notifications: false
            }
            if(this.pendingRequestsCb !== null) {
                this.pendingRequestsCb();
            }
            this.storeSettings();
        }
    }

    public setLastUsername(lastUsername : string) : void { 
        this.settingsObject.lastUsername = lastUsername;
        this.storeSettings();

        if(this.pendingRequestsCb !== null) {
            this.pendingRequestsCb();
        }
    }
    public getLastUsername() : string { return this.settingsObject.lastUsername; };

    public addCredential(username : string, credentialId : string) : void {
        this.settingsObject.credentials[username] = credentialId;
        this.storeSettings();
    }
    
    public getCredential(username : string) : any { return this.settingsObject.credentials[username]; }
    public hasCredential(username : string) : boolean { return (typeof(this.settingsObject.credentials[username]) !== "undefined")};;
 
    private storeSettings() : void {
        Store.set(this.currentAddr, this.settingsObject).catch((err) => {
            console.error("[SETTINGS] Error saving data: "+err);
        })
    }

    private getDefaultObject() : ISettings {
        
        return {
            userData: {},
            lastUsername: "",
            credentials: {}
        }
    }

    private currentAddr : string = "";
    private settingsObject : (ISettings);

    private contactsChangedCb : (() => any) | null = null;
    private chatChangedCb : (() => any) | null = null;
    private pendingRequestsCb : (() => any) | null = null;
}

const CurrentSettings = new Settings();
export default CurrentSettings;