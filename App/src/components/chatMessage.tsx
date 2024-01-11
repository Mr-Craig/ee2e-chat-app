import { IonApp, IonHeader, IonToolbar, IonTitle, IonContent, IonText, IonButtons, IonBackButton, IonSearchbar, IonItem, IonLabel, IonImg, IonRefresher, IonRefresherContent, RefresherEventDetail, useIonLoading, useIonAlert, IonNote, useIonViewDidEnter, useIonViewDidLeave, withIonLifeCycle, IonIcon, IonButton } from "@ionic/react";
import { Icon } from "ionicons/dist/types/components/icon/icon";
import { personAdd } from "ionicons/icons";
import { useEffect, useState } from "react";
import { useHistory } from "react-router";
import Chat, { messageStateEnumToString } from "../helpers/classes/chat";
import CurrentSettings from "../helpers/settings";
import Socket from "../helpers/socket";
import toastController from "../helpers/toastController";
import { EContactState, EMessageSender, EMessageState, IContact, IMessage } from "../helpers/types";

interface IChatMessage
{
    message: IMessage,
    chatData: Chat
}

const ChatMessage : React.FC<IChatMessage> = (props) => {
    const history = useHistory();

    useEffect(() => {
        if(props.message.state !== EMessageState.READ && props.message.who === EMessageSender.RECEIVER) {
            Socket.relay(props.chatData.getUsername(), "messageState", {
                username: Socket.getSocketUsername(),
                messageId: props.message.id,
                state: EMessageState.READ
            });
            props.chatData.setMessageState(props.message.id, EMessageState.READ);
        }
    }, []);
    
    return (
        <IonItem lines="full" style={{
            margin: "10px 0px"
        }}>
            <IonLabel slot={props.message.who === EMessageSender.SENDER ? "end" : "start"} style={{
                textAlign: props.message.who === EMessageSender.SENDER ? "right" : "left",
                marginBottom: "10px"
            }}>
                <p>{props.message.who === EMessageSender.SENDER ? Socket.getSocketUsername() : props.chatData.getUsername()}</p>
                <h2 className="ion-text-wrap">{props.message.contents}</h2>
            </IonLabel>
            <IonNote slot={props.message.who === EMessageSender.SENDER ? "start" : "end"} style={{
                textAlign: props.message.who === EMessageSender.SENDER ? "left" : "right"
            }}>
                {new Date(props.message.time).toLocaleDateString() === new Date().toLocaleDateString() ? new Date(props.message.time).toLocaleTimeString([], {hour: '2-digit', minute:'2-digit'}) : new Date(props.message.time).toLocaleDateString()}
                <br/>
                {props.message.who === EMessageSender.SENDER && <IonLabel>{messageStateEnumToString(props.message.state)}</IonLabel>}
            </IonNote>
        </IonItem>
    );
}

export default ChatMessage;