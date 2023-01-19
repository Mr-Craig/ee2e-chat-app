import { IonApp, IonHeader, IonToolbar, IonTitle, IonContent, IonText, IonButtons, IonBackButton, IonSearchbar, IonItem, IonLabel, IonImg, IonIcon, IonGrid, IonRow, IonCol, IonButton, IonRippleEffect, IonInput, IonPage, IonFooter, useIonModal, useIonLoading, useIonAlert } from "@ionic/react";
import { useEffect, useState } from "react";
import { chatbubbles } from "ionicons/icons";

import "./splash.css";
import { Link } from "react-router-dom";
import Socket from "../../helpers/socket";

const SplashPage : React.FC = () => {
    const [server, setServer] = useState("");
    const [present, dismiss] = useIonLoading();
    const [presentAlert] = useIonAlert();
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
            </IonContent>
            <IonFooter class="ion-padding" style={{
                marginBottom: "10%"
            }}>
            <IonGrid>
                    <IonRow>
                        <IonCol>
                            <IonItem className="ion-no-padding" style={{
                                background: "none"
                            }}>
                                <IonLabel position="floating">Server Name</IonLabel>
                                <IonInput type="text" onIonInput={(event) => setServer(event.target.value as string)}></IonInput>
                            </IonItem>
                        </IonCol>
                    </IonRow>
                    <IonRow>
                        <IonCol>
                            <IonButton class="ion-activatable ripple-parent" onClick={(event) => {
                                    present({
                                        message:"Connecting...",
                                        spinner:"circles"
                                    });
                                    Socket.connect(server).then((res) => {
                                        if(!res) {
                                            dismiss();
                                            presentAlert({
                                                header: 'Failed',
                                                message: 'Failed to connect to server, please try a different one.',
                                                buttons: ['OK']
                                            });
                                        } else {
                                            dismiss();
                                        }
                                    });
                            }} expand="block">
                                Connect
                                <IonRippleEffect type="unbounded"></IonRippleEffect>
                            </IonButton>
                        </IonCol>
                    </IonRow>
                    <IonRow>
                        <IonCol>
                            <IonText  className="ion-no-padding" style={{
                                    fontSize: "18px",
                                    textAlign: "center",
                                    position: "absolute",
                                    width: "calc(100% - var(--ion-padding, 16px))",
                                }}>
                                <Link to="/servers" style={{ textDecoration: 'none', width: "100%", textAlign: "center", paddingTop: "15px" }}>
                                    Server List
                                </Link>
                            </IonText>
                        </IonCol>
                    </IonRow>
                </IonGrid>
            </IonFooter>
        </IonPage>
    );
}

export default SplashPage;