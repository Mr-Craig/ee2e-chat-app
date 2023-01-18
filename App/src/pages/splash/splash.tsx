import { IonApp, IonHeader, IonToolbar, IonTitle, IonContent, IonText, IonButtons, IonBackButton, IonSearchbar, IonItem, IonLabel, IonImg, IonIcon, IonGrid, IonRow, IonCol, IonButton, IonRippleEffect, IonInput, IonPage } from "@ionic/react";
import { useEffect, useState } from "react";
import { chatbubbles } from "ionicons/icons";

import "./splash.css";
import { Link } from "react-router-dom";

const SplashPage : React.FC = () => {
    return (
        <IonPage>
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
                    marginTop: "auto"
                }}>
                    <IonRow>
                        <IonCol>
                            <IonItem className="ion-no-padding">
                                <IonLabel position="stacked">Server Name</IonLabel>
                                <IonInput></IonInput>
                            </IonItem>
                        </IonCol>
                    </IonRow>
                    <IonRow>
                        <IonCol>
                            <IonButton class="ion-activatable ripple-parent" expand="block">
                                Connect
                                <IonRippleEffect type="unbounded"></IonRippleEffect>
                            </IonButton>
                        </IonCol>
                    </IonRow>
                    <IonRow>
                        <IonCol>
                            <IonText  className="ion-no-padding" style={{
                                    fontSize: "18px",
                                    textAlign: "center"
                                }}>
                                <Link to="/servers" style={{ textDecoration: 'none', width: "100%", textAlign: "center", paddingTop: "15px" }}>
                                    Server List
                                </Link>
                            </IonText>
                        </IonCol>
                    </IonRow>
                </IonGrid>

            </IonContent>
        </IonPage>
    );
}

export default SplashPage;