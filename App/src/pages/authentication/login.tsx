import { IonApp, IonHeader, IonToolbar, IonTitle, IonContent, IonText, IonButtons, IonBackButton, IonSearchbar, IonItem, IonLabel, IonImg, IonIcon, IonGrid, IonRow, IonCol, IonButton, IonRippleEffect, IonInput } from "@ionic/react";
import { useEffect, useState } from "react";
import { chatbubbles } from "ionicons/icons";

import { Link } from "react-router-dom";

const LoginPage : React.FC = () => {
    return (
        <IonApp>
            <IonHeader>
                <IonToolbar>
                    <IonTitle>Login</IonTitle>
                    <IonButtons slot="start">
                        <IonBackButton defaultHref="/" />
                    </IonButtons>
                </IonToolbar>
            </IonHeader>
            <IonContent>
            </IonContent>
        </IonApp>
    );
}

export default LoginPage;