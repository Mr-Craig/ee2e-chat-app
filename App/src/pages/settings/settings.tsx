import { IonApp, IonHeader, IonToolbar, IonTitle,useIonViewWillEnter, IonContent, IonText, IonButtons, IonBackButton, IonSearchbar, IonItem, IonLabel, IonImg, IonIcon, IonGrid, IonRow, IonCol, IonButton, IonRippleEffect, IonInput, IonPage, IonFooter, useIonModal, useIonLoading, useIonAlert, useIonViewDidEnter, IonCheckbox } from "@ionic/react";
import { useEffect, useState } from "react";
import { chatbubbles, exit } from "ionicons/icons";

import { Link } from "react-router-dom";
import Socket from "../../helpers/socket";
import { hideTabBar, showTabBar } from "../../App";
import CurrentSettings from "../../helpers/settings";
import toastController from "../../helpers/toastController";

const SettingsPage : React.FC = () => {
    const [presentAlert] = useIonAlert();
    const [notifsEnabled, setNotifsEnabled] = useState<boolean>(CurrentSettings.getNotificationsValue());

    useIonViewDidEnter(() => {
        showTabBar();
    });
    useIonViewWillEnter(() => {
        showTabBar();
    });

    const checkChange = (checked : boolean) => {
        if ('serviceWorker' in navigator) {
            Notification.requestPermission().then((val) => {
                if(val === 'granted') {
                    navigator.serviceWorker.controller?.postMessage({
                        type: checked ? 'NOTIF_ON' : 'NOTIF_OFF',
                        key: Socket.getServerKey()
                    });
                    CurrentSettings.setNotificationsValue(checked);
                    setNotifsEnabled(checked);
                } else {
                    toastController.sendToast({
                        message:"You denied permissions for notifications.",
                        color: "danger",
                        duration: 1000
                    });
                    setNotifsEnabled(false);
                }
            })
        }
    }

    return (
        <IonPage>
            <IonHeader>
                <IonToolbar>
                    <IonTitle>Settings</IonTitle>
                </IonToolbar>
            </IonHeader>
            <IonContent class="ion-padding">
                <IonItem>
                    <IonLabel>Username: {Socket.getSocketUsername()}</IonLabel>
                </IonItem>
                <IonItem>
                    <IonCheckbox slot="start" checked={notifsEnabled} onIonChange={(e) => checkChange(e.target.checked)}></IonCheckbox>
                    <IonLabel>Enable Notifications</IonLabel>
                </IonItem>
                <IonItem button onClick={(event) => {
                    presentAlert({
                        header:"Are you sure you want to disconnect?",
                        buttons:[
                            {
                                text:'No',
                                role: 'cancel'
                            },
                            {
                                text:"Yes",
                                role: "confirm",
                                handler: () => {
                                    Socket.close();
                                }
                            }
                        ]
                    })
                }}>
                    <IonIcon icon={exit} slot="start"/>
                    <IonLabel color="danger">Disconnect</IonLabel>
                </IonItem>
            </IonContent>
        </IonPage>
    );
}

export default SettingsPage;