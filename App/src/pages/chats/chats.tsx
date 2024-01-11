import { IonApp, IonHeader, IonToolbar, IonTitle, IonContent, IonText, IonButtons, IonBackButton, IonSearchbar, IonItem, IonLabel, IonImg, IonIcon, IonGrid, IonRow, IonCol, IonButton, IonRippleEffect, IonInput, IonPage, IonFooter, useIonModal, useIonLoading, useIonAlert, useIonViewDidEnter, useIonViewWillEnter, IonList, IonicSwiper, IonItemSliding, IonItemOption, IonItemOptions, IonNote } from "@ionic/react";
import { useEffect, useMemo, useReducer, useState } from "react";
import { bicycle, chatbubbles, ellipse, trashBin } from "ionicons/icons";

import { Link, useHistory } from "react-router-dom";
import Socket from "../../helpers/socket";
import { hideTabBar, showTabBar } from "../../App";
import CurrentSettings from "../../helpers/settings";
import { EChatState, EContactState, EMessageSender, EMessageState } from "../../helpers/types";
import { messageStateEnumToString } from "../../helpers/classes/chat";


interface IChatsPreview {
    username : string,
    lastMessage : string,
    lastMessageTime : number,
    lastMessageTimeString : string
    who: EMessageSender,
    state: EMessageState
}
const ChatsPage : React.FC = () => {
    const history = useHistory();
    const [searchTerm, setSearchTerm] = useState<string>("");
    const [presentAlert] = useIonAlert();

    // force a re-render 
    const [forcedReRender, forceUpdate] = useReducer(x => x + 1, 0);
    
    CurrentSettings.setChatCallback(() => {
        forceUpdate();
    });

    useIonViewDidEnter(() => {
        showTabBar();
    });
    useIonViewWillEnter(() => {
        showTabBar();
    });

    const chats = useMemo<IChatsPreview[]>(() => {
        return CurrentSettings.getChats()
            .filter((value) => Object.keys(value.messages).length > 0 && value.username.search(searchTerm) !== -1)
            .map((value) => {
                let allMessages = Object.values(value.messages);
                let lastMessage = allMessages[allMessages.length-1];
                let lastMessageDate : Date = new Date(lastMessage.time);

                return {
                    username: value.username,
                    lastMessage: lastMessage.contents,
                    lastMessageTime: lastMessage.time,
                    lastMessageTimeString: lastMessageDate.toLocaleDateString() === new Date().toLocaleDateString() ? lastMessageDate.toLocaleTimeString([], {hour: '2-digit', minute:'2-digit'}) : lastMessageDate.toLocaleDateString(),
                    who: lastMessage.who,
                    state: lastMessage.state
                }
            })
            .sort((a,b) => b.lastMessageTime - a.lastMessageTime);
    }, [searchTerm, forcedReRender]);

    const deleteChat = async (chatId : string) => {
        presentAlert({
            header:"Are you sure you want to delete this chat? This can't be undone.",
            buttons:[
                {
                    text:'No',
                    role: 'cancel'
                },
                {
                    text:"Yes",
                    role: "confirm",
                    handler: () => {
                        CurrentSettings.deleteChat(chatId);
                    }
                }
            ]
        })
    };

    return (
        <IonPage>
            <IonHeader>
                <IonToolbar>
                    <IonTitle>Chats</IonTitle>
                </IonToolbar>
                <IonToolbar>
                    <IonSearchbar onIonInput={(event) => setSearchTerm(event.target.value as string)} value={searchTerm}></IonSearchbar>
                </IonToolbar>
            </IonHeader>
            <IonContent>
                <IonList>
                    {chats.map((value, index) => {
                        return (
                            <IonItemSliding key={index}>
                                <IonItem style={{
                                    "--background": "var(--ion-item-background, var(--ion-background-color, #fff))"
                                }} onClick={() => {
                                    history.push(`/chat/${value.username}`);
                                }}>
                                    <IonLabel>
                                        <h2>{value.username}</h2>
                                        <p style={{
                                            fontWeight: value.who === EMessageSender.RECEIVER && value.state !== EMessageState.READ ? 'bold' : 'normal'
                                        }}>
                                            <span style={{
                                                color: "var(--ion-color-primary, #fff)"
                                            }}>{value.who === EMessageSender.RECEIVER && value.state !== EMessageState.READ && "⬤"} </span> 
                                            {value.lastMessage}
                                        </p>
                                    </IonLabel>
                                    <IonNote style={{
                                        marginTop: "auto",
                                        marginBottom: "auto",
                                        textAlign: "right"
                                    }}>
                                        {value.lastMessageTimeString}
                                        <br/>
                                        {value.who === EMessageSender.SENDER && messageStateEnumToString(value.state)}
                                    </IonNote>
                                </IonItem>
                                <IonItemOptions onIonSwipe={() => {
                                    deleteChat(value.username);
                                }}>
                                    <IonItemOption color="danger" expandable onClick={() => {
                                        deleteChat(value.username);
                                    }}>
                                        <IonIcon icon={trashBin} slot="start"/>
                                        Delete
                                    </IonItemOption>
                                </IonItemOptions>
                            </IonItemSliding>
                        );
                    })}
                </IonList>
            </IonContent>
        </IonPage>
    );
}

export default ChatsPage;