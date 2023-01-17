import { IonApp, IonHeader, IonToolbar, IonTitle, IonContent, IonText, IonButtons, IonBackButton, IonSearchbar, IonItem, IonLabel, IonImg, IonIcon, IonGrid, IonRow, IonCol, IonButton, IonRippleEffect, IonInput } from "@ionic/react";
import { useEffect, useState } from "react";
import { chatbubbles } from "ionicons/icons";

import "./splash.css";
import { Link } from "react-router-dom";

const SplashPage : React.FC = () => {
    return (
        <IonApp>
            <IonContent class="ion-padding">
                <IonGrid>
                    <IonRow class="ion-justify-content-center">
                        <IonCol size="6">
                            <IonIcon id="chat-icon" icon={chatbubbles}></IonIcon>
                        </IonCol>
                    </IonRow>
                    <IonRow>
                        <IonCol>
                            <h1 style={{
                                "textAlign": "center",
                                "fontWeight": "bold",
                                "fontSize": "32px"
                            }}>E2EE Chat App</h1>
                            <p style={{
                                "textAlign": "center"
                            }}>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam tempus dictum sem sed pulvinar. Nam fermentum justo et orci sodales venenatis in a justo. Duis vulputate justo neque, eget semper tellus efficitur at.</p>
                        </IonCol>
                    </IonRow>
                </IonGrid>

                <IonGrid style={{
                    "bottom": "0",
                    "position": "absolute",
                    "width": "calc(100% - var(--ion-padding, 16px))",
                    marginLeft: "calc(0px - calc(var(--ion-padding, 16px) / 2))",
                    paddingBottom: "10%"
                }}>
                    <IonRow>
                        <IonItem style={{
                            "width": "calc(100% - var(--ion-padding, 16px))",
                            marginLeft: "0px",
                            paddingBottom: "5%"
                        }}>
                            <IonLabel position="stacked">Server Name</IonLabel>
                            <IonInput></IonInput>
                        </IonItem>
                        <IonButton expand="full"  style={{
                            "width": "100%"
                        }} class="ion-activatable ripple-parent">
                            Connect
                            <IonRippleEffect type="unbounded"></IonRippleEffect>
                        </IonButton>

                        <Link to="/servers" style={{ textDecoration: 'none', width: "100%", textAlign: "center", paddingTop: "15px" }}>
                            <IonText style={{
                                fontSize: "18px",
                            }}> Server List </IonText>
                        </Link>
                    </IonRow>
                </IonGrid>

            </IonContent>
        </IonApp>
    );
}

export default SplashPage;