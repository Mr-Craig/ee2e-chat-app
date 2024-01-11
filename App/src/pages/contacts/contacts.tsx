import { IonApp, IonHeader, IonToolbar,IonSegmentButton, IonTitle, IonContent, IonText, IonButtons, IonBackButton, IonSearchbar, IonItem, IonLabel, IonImg, IonIcon, IonGrid, IonRow, IonCol, IonButton, IonRippleEffect, IonInput, IonPage, IonFooter, useIonModal, useIonLoading, useIonAlert, useIonViewDidEnter, IonSegment, IonList, IonBadge, useIonViewWillEnter } from "@ionic/react";
import { useEffect, useState, useReducer } from "react";
import { chatbubbles, ellipse } from "ionicons/icons";

import { Link } from "react-router-dom";
import Socket from "../../helpers/socket";
import { hideTabBar, showTabBar } from "../../App";
import CurrentSettings from "../../helpers/settings";
import ContactEntry from "../../components/contactEntry";
import toastController from "../../helpers/toastController";
import { EContactState } from "../../helpers/types";

const ContactsPage : React.FC = () => {
    const [presentAlert] = useIonAlert();
    const [segmentValue, setSegmentValue] = useState<string>("available");

    // force a re-render 
    const [ignored, forceUpdate] = useReducer(x => x + 1, 0);

    useIonViewDidEnter(() => {
        showTabBar();
    });
    useIonViewWillEnter(() => {
        showTabBar();
    });

    CurrentSettings.setContactCallback(() => {
        forceUpdate();
    });

    const addContact = (username : string) => {
        if(username === Socket.getSocketUsername()) {
            toastController.sendToast({
                message: `You can't add yourself! sorry!`,
                duration: 1000,
                color: "danger"
            });
            return;
        }
        if(CurrentSettings.doesContactExist(username)) {
            toastController.sendToast({
                message: `You've already got ${username} in your contacts`,
                duration: 1000,
                color: "danger"
            });
            return;
        }
        CurrentSettings.addContact({
            username: username,
            contactState: EContactState.SENT
        });
        

        Socket.relay(username, "chatRequest", {
            username: Socket.getSocketUsername(),
            contactState: EContactState.SENT
        });

        toastController.sendToast({
            message: `Sent ${username} a request`,
            duration: 1000,
            color: "success"
        });
    };

    //console.log(CurrentSettings.getContacts());
    return (
        <IonPage>
            <IonHeader>
                <IonToolbar>
                    <IonTitle>Contacts</IonTitle>
                </IonToolbar>
                <IonToolbar>
                    <IonSegment value={segmentValue} onIonChange={(event) => {
                        setSegmentValue(event.detail.value as string);
                    }}>
                        <IonSegmentButton value="available">
                            Available
                        </IonSegmentButton>
                        <IonSegmentButton value="pending" layout="icon-end">
                            <IonLabel>Pending</IonLabel>
                            {CurrentSettings.getContacts().filter(val => val.contactState === EContactState.PENDING).length > 0 &&
                                <IonIcon icon={ellipse} color="primary" size="small"/>}
                        </IonSegmentButton>
                </IonSegment>
                </IonToolbar>
            </IonHeader>
            <IonContent class="ion-no-padding">
                {segmentValue === "pending" ? CurrentSettings.getContacts().sort((a, b) => a.contactState - b.contactState).filter((i) => i.contactState !== EContactState.ACCEPTED).map((value, index) => {
                    return (<ContactEntry key={index} contact={value} />);
                }) : CurrentSettings.getContacts().sort((a, b) => a.contactState - b.contactState).filter((i) => i.contactState === EContactState.ACCEPTED).map((value, index) => {
                    return (<ContactEntry key={index} contact={value} />);
                })
                }
            </IonContent>
            <IonFooter class="ion-padding">
                <IonButton expand="block" onClick={(event) => {
                    presentAlert({
                        header: "Enter the username",
                        buttons: [{
                            text:"Submit",
                            handler: (input) => {
                                addContact(input[0]);
                            }
                        }, {
                            text:"Cancel"
                        }],
                        inputs: [
                            {
                                placeholder: "Username",
                                attributes: {
                                    "autocapitalize": "off"
                                }
                            }
                        ]
                    })
                }}>
                    Add a New Contact
                </IonButton>
            </IonFooter>
        </IonPage>
    );
}

export default ContactsPage;