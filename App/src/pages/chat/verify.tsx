import { IonApp, IonHeader, IonToolbar, IonTitle, IonContent, IonText, IonButtons, IonBackButton, IonSearchbar, IonItem, IonLabel, IonImg, IonIcon, IonGrid, IonRow, IonCol, IonButton, IonRippleEffect, IonInput, IonPage, IonFooter, useIonModal, useIonLoading, useIonAlert, useIonViewDidEnter, useIonViewWillEnter, IonList, IonTextarea, IonNote, getPlatforms, isPlatform, useIonViewWillLeave, IonModal } from "@ionic/react";
import { useEffect, useState, useReducer, useRef, useMemo} from "react";
import { arrowForwardCircle, chatbubbles, lockClosed } from "ionicons/icons";

import { Link, useHistory, useParams} from "react-router-dom";
import Socket from "../../helpers/socket";
import { hideTabBar, showTabBar } from "../../App";
import CurrentSettings from "../../helpers/settings";
import {Crypto} from "capacitor-crypto";
import toastController from "../../helpers/toastController";
import uniqid from 'uniqid';
import { EChatState, EContactState, EMessageSender, EMessageState, IChat, IMessage } from "../../helpers/types";
import Chat from "../../helpers/classes/chat";
import ChatMessage from "../../components/chatMessage";
import * as QRCode from 'qrcode';
import QrScanner from 'qr-scanner';

import { BarcodeScanner } from '@awesome-cordova-plugins/barcode-scanner';

interface IChatParams
{
    username : string
}

const VerifyChatPage : React.FC = () => {
    const [present] = useIonAlert();

    const {username} = useParams<IChatParams>();
    const [qr, setQR] = useState<string>();
    const [hash, setHash] = useState<string>("");

    const videoElem = useRef<HTMLVideoElement>(null);
    const modal = useRef<HTMLIonModalElement>(null);
    const [scanning, setScanning] = useState<boolean>(false);

    const history = useHistory();

    let qrScanner : QrScanner | null = null;

    const verifyCode = useMemo(() => {
        //https://stackoverflow.com/a/34356351
        function hexToBytes(hex : string) : Uint8Array {
            for (var bytes : number[] = [], c = 0; c < hex.length; c += 2)
                bytes.push(parseInt(hex.substr(c, 2), 16));
            return new Uint8Array(bytes);
        }

        const numbers = hexToBytes(hash);
        
        let numberString = "";

        for(let i = 0; i < numbers.length; i += 8) {
            let sliceArray = numbers.slice(i, i+8);
            let dataView = new DataView(sliceArray.buffer);
            let uint64 = dataView.getBigUint64(0, true);
            numberString += `${uint64} `
        }

        return numberString;
    }, [hash]);

    const openScanner = async () => {
        if(isPlatform('cordova')) {
            const data = await BarcodeScanner.scan({   
            });
            if(data.text.length > 0) {
                present({
                    message: data.text === hash ? "Successfully verified!" : `Failed to verify, Got ${data.text} should be ${hash}`,
                    buttons: ['OK']
                })
            }
        } else {
            setScanning(true);
        }
      };
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

        let chatData = CurrentSettings.getChat(username);

        if(chatData.getState() === EChatState.KEYEXCHANGE) {
            history.goBack();
            toastController.sendToast({
                message: "Key Exchange not complete"
            });
            return;
        }

        Crypto.hash({
            data: chatData.getSharedSecret()
        }).then((res) => {
            setHash(res.hash);
            
            QRCode.toDataURL(res.hash).then((url) => {
                setQR(url);
            });
        })
    }, []);

    useIonViewDidEnter(() => {
        hideTabBar();
    })
    useIonViewWillEnter(() => {
        hideTabBar();
    });

    useIonViewWillLeave(() => {
        qrScanner?.stop();
        qrScanner?.destroy();
        qrScanner = null;
        setScanning(false);
    });

    return (
        <IonPage>
            <IonHeader>
                <IonToolbar>
                    <IonTitle>Verify Key - {username}</IonTitle>
                    <IonButtons slot="start">
                        <IonBackButton defaultHref={`/chat/${username}`}></IonBackButton>
                    </IonButtons>
                </IonToolbar>
            </IonHeader>
            <IonContent class="ion-padding">
                <IonImg src={qr} style={{
                    height: "40vh"
                }}/>
                <IonItem>
                    <IonLabel>
                        <h2 style={{
                            textAlign: "center"
                        }}>Verification Code</h2>
                        <p className="ion-text-wrap" style={{
                            textAlign: "center"
                        }}>{verifyCode}</p>
                    </IonLabel>
                </IonItem>
                <IonItem>
                    <IonLabel className="ion-text-wrap" style={{
                        textAlign: "center"
                    }}>
                        You can ensure that your chats are end-to-end encrypted by confirming that the other party has the same verification code as you.
                    </IonLabel>
                </IonItem>
                <IonModal ref={modal} isOpen={scanning} onDidPresent={() => {
                    qrScanner?.stop();
                    qrScanner?.destroy();
                    qrScanner = new QrScanner(
                        videoElem.current as HTMLVideoElement,
                        (result) =>  {
                            present({
                                message: result.data === hash ? "Successfully verified! Your messages are end-to-end encrypted." : `Failed to verify, Got ${result.data} should be ${hash}`,
                                buttons: ['OK']
                            })
                            qrScanner?.stop();
                            qrScanner?.destroy();
                            qrScanner = null;
                            setScanning(false);
                        },
                        { /* your options or returnDetailedScanResult: true if you're not specifying any other options */ },
                    );
                    qrScanner?.start();
                }} onWillDismiss={() => {
                    console.log("DESTROYED");
                    qrScanner?.stop();
                    qrScanner?.destroy();
                    qrScanner = null;
                }}>
                    <IonHeader>
                        <IonToolbar>
                            <IonButtons slot="start">
                                <IonButton onClick={() => setScanning(false)}>X</IonButton>
                            </IonButtons>
                            <IonTitle>Scan QR Code</IonTitle>
                        </IonToolbar>
                    </IonHeader>
                    <IonContent>
                        <video ref={videoElem} style={{
                            width: "100%",
                            margin: 'auto 0'
                        }}></video>
                    </IonContent>
                </IonModal>
            </IonContent>
            <IonFooter className="ion-padding">
                <IonButton onClick={openScanner} expand="block">Scan QR Code</IonButton>
            </IonFooter>
        </IonPage>
    );
}

export default VerifyChatPage;