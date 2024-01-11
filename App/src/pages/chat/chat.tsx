import { IonApp, IonHeader, IonToolbar, IonTitle, IonContent, IonText, IonButtons, IonBackButton, IonSearchbar, IonItem, IonLabel, IonImg, IonIcon, IonGrid, IonRow, IonCol, IonButton, IonRippleEffect, IonInput, IonPage, IonFooter, useIonModal, useIonLoading, useIonAlert, useIonViewDidEnter, useIonViewWillEnter, IonList, IonTextarea, IonNote } from "@ionic/react";
import { useEffect, useState, useReducer, useRef} from "react";
import { arrowForwardCircle, chatbubbles, lockClosed } from "ionicons/icons";

import { Link, useHistory, useParams} from "react-router-dom";
import Socket from "../../helpers/socket";
import { hideTabBar, showTabBar } from "../../App";
import CurrentSettings from "../../helpers/settings";
import toastController from "../../helpers/toastController";
import uniqid from 'uniqid';
import {Crypto} from 'capacitor-crypto';
import { EChatState, EContactState, EMessageSender, EMessageState, IChat, IMessage } from "../../helpers/types";
import Chat from "../../helpers/classes/chat";
import ChatMessage from "../../components/chatMessage";

interface IChatParams
{
    username : string
}

const ChatPage : React.FC = () => {
    const [present] = useIonAlert();

    const {username} = useParams<IChatParams>();
    const [chatUsername, setChatUsername] = useState<string>("");
    const [message, setNewMessage] = useState<string>("");
    const [chatData, setChatData] = useState<Chat>();
    const bottom = useRef<HTMLDivElement>(null);

    // force a re-render 
    const [forcedReRender, forceUpdate] = useReducer(x => x + 1, 0);

    const history = useHistory();

    CurrentSettings.setChatCallback(() => {
        forceUpdate();
    });

    useEffect(() => {
        bottom.current?.scrollIntoView({behavior: 'smooth'});
    }, [forcedReRender]);

    useEffect(() => {
        console.log(bottom.current);
        bottom.current?.scrollIntoView({behavior: 'smooth'});
    }, []);

    useEffect(() => {
        if(!CurrentSettings.doesContactExist(username)) {
            history.goBack();
            toastController.sendToast({
                message: "This person isn't in your contacts."
            });
            return;
        }

        if(CurrentSettings.getContactState(username) !== EContactState.ACCEPTED) {
            history.goBack();
            toastController.sendToast({
                message: "Contact not accepted yet."
            });
            return;
        }

        setChatUsername(username);
        setChatData(CurrentSettings.getChat(username));
    }, []);

    useEffect(() => {
        if(typeof(chatData) === "undefined")
            return;
        if(chatData.getState() === EChatState.KEYEXCHANGE && chatData.getKeys().privateKey.length <= 0) {
            Crypto.generateKeyPair().then((keyPair) => {
                chatData.setKeys(keyPair);
                Socket.relay(chatData.getUsername(), "keyExchange", {
                    username: Socket.getSocketUsername(),
                    publicKey: keyPair.publicKey
                });
            }).catch((err) => {
                console.log(err);
            })
        }
    }, [chatData]);

    useIonViewDidEnter(() => {
        hideTabBar();
    })
    useIonViewWillEnter(() => {
        hideTabBar();
    });

    const sendMessage = async () => {
        if(chatData?.getState() !== EChatState.READY) {
            setNewMessage("");
            return;
        }

        // append our username in rare case the id is the name
        const messageId = uniqid(Socket.getSocketUsername());
        const newMessage : IMessage = {
            id: messageId,
            contents: message,
            time: new Date().getTime(),
            state: EMessageState.SENT,
            who: EMessageSender.SENDER
        };

        const messageEncrypted = await Crypto.encrypt({
            key: chatData.getSharedSecret(),
            data: message,
        });

        chatData.addMessage(messageId, newMessage);

        Socket.relay(chatData.getUsername(), "newMessage", {
            username: Socket.getSocketUsername(),
            id: messageId,
            time: newMessage.time,
            encryptedData: messageEncrypted.encryptedData,
            eTag: messageEncrypted.tag,
            iv: messageEncrypted.iv
        });

        setNewMessage("");
    };

    return (
        <IonPage>
            <IonHeader>
                <IonToolbar>
                    <IonTitle>{username}</IonTitle>
                    <IonButtons slot="start">
                        <IonBackButton></IonBackButton>
                    </IonButtons>
                    <IonButtons slot="end" className="ion-padding" onClick={() => {
                        if(chatData?.getState() === EChatState.READY) {
                            history.push(`/chatverify/${username}`);
                        } else {
                            present({
                                message: "Key Exchange not completed yet, no messages will be sent untill it's complete.",
                                buttons: ["OK"]
                            });
                        }
                    }}>
                        <IonIcon icon={lockClosed} color={chatData?.getState() === EChatState.KEYEXCHANGE ? "warning" : "success"}/>
                    </IonButtons>
                </IonToolbar>
            </IonHeader>
            <IonContent class="ion-no-padding">
                {chatData?.getMessages().map((msg) => {
                    return (<ChatMessage key={msg.id} message={msg} chatData={chatData}/>);
                })}
                <div ref={bottom}></div>
            </IonContent>
            <IonFooter>
                <IonToolbar>
                <IonItem>
                    <IonTextarea placeholder="Send a message..." value={message} onKeyUp={(event) => {
                        if(event.key === "Enter") {
                            (event.target as HTMLElement).blur();
                            sendMessage();
                        }
                    }} onIonInput={(event) => {
                        setNewMessage(event.target.value as string);
                    }}></IonTextarea>
                    <IonButton onClick={() => {
                        sendMessage();
                    }} style={{
                        marginTop:"auto",
                        marginBottom: "auto"
                    }}>
                        <IonIcon icon={arrowForwardCircle}/>
                    </IonButton>
                </IonItem>
                </IonToolbar>
            </IonFooter>
        </IonPage>
    );
}

export default ChatPage;