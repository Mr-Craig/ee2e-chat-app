import { IonApp, IonHeader, IonToolbar, IonTitle, IonContent, IonText, IonButtons, IonBackButton, IonSearchbar, IonItem, IonLabel, IonImg, IonRefresher, IonRefresherContent, RefresherEventDetail, useIonLoading, useIonAlert, IonNote, useIonViewDidEnter, useIonViewDidLeave, withIonLifeCycle, IonIcon, IonButton } from "@ionic/react";
import { Icon } from "ionicons/dist/types/components/icon/icon";
import { personAdd } from "ionicons/icons";
import { useEffect, useState } from "react";
import { useHistory } from "react-router";
import CurrentSettings from "../helpers/settings";
import Socket from "../helpers/socket";
import toastController from "../helpers/toastController";
import { EContactState, IContact } from "../helpers/types";

interface IContactEntry
{
    contact : IContact
}

const ContactEntry : React.FC<IContactEntry> = (props) => {
    const history = useHistory();
    const acceptRequest = () => {
        if(CurrentSettings.editContactState(props.contact.username, EContactState.ACCEPTED)) {
            // do stuff
            Socket.relay(props.contact.username, "chatRequest", {
                username: Socket.getSocketUsername(),
                contactState: EContactState.ACCEPTED
            } as IContact);
            toastController.sendToast({
                message: `You are now friends with ${props.contact.username}`,
                duration: 1000,
                color: "success"
            });
        } else {
            // failed to delete contact (maybe already accepted?)
            toastController.sendToast({
                message: "Failed to accept request",
                duration: 1000,
                color: "danger"
            });
        }
    };
    const denyRequest = () => {
        if(CurrentSettings.editContactState(props.contact.username, EContactState.DENIED)) {
            // do stuff
            Socket.relay(props.contact.username, "chatRequest", {
                username: Socket.getSocketUsername(),
                contactState: EContactState.DENIED
            } as IContact);
            toastController.sendToast({
                message: `Successfully denied request from ${props.contact.username}`,
                duration: 1000,
                color: "success"
            });
        } else {
            // failed to delete contact (maybe already denied?)
            toastController.sendToast({
                message: "Failed to deny request",
                duration: 1000,
                color: "danger"
            });
        }
    };
    return (
        <IonItem button={props.contact.contactState === EContactState.ACCEPTED} onClick={() => {
            if(props.contact.contactState === EContactState.ACCEPTED) {
                history.push(`/chat/${props.contact.username}`);
            }
        }}>
            <IonIcon icon={personAdd} slot="start"/>
            <IonLabel>
                {props.contact.username}
            </IonLabel>

            {props.contact.contactState === EContactState.PENDING ? (
                <IonButtons>
                    <IonButton color="success" onClick={acceptRequest}>Y</IonButton>
                    <IonButton color="danger" onClick={denyRequest}>X</IonButton>
                </IonButtons>
            ) : (
                <IonNote slot="end">
                    {props.contact.contactState === EContactState.ACCEPTED ? "Accepted" : "Pending"}
                </IonNote>
            )}
        </IonItem>
    );
}

export default ContactEntry;