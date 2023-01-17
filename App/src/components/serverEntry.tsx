import { IonApp, IonHeader, IonToolbar, IonTitle, IonContent, IonText, IonButtons, IonBackButton, IonSearchbar, IonItem, IonLabel, IonImg, IonRefresher, IonRefresherContent, RefresherEventDetail, useIonLoading, useIonAlert, IonNote, useIonViewDidEnter, useIonViewDidLeave, withIonLifeCycle } from "@ionic/react";
import { useEffect, useState } from "react";
import Socket from "../helpers/socket";

export interface serverEntry 
{
    geolocation : string,
    serverIp : string
}


interface serverEntryFC
{
    serverEntry : serverEntry,
    onClick : (arg0 : serverEntry) => void
}

const ServerEntry : React.FC<serverEntryFC> = (props) => {
    const [ping, setPing] = useState("-");
    
    useEffect(() => {
        const t0 = performance.now();
                fetch(`https://${props.serverEntry.serverIp}/ping`)
                .then((response) => response.text())
                .then((data) => {
                    const t1 = performance.now();
                    setPing(`${(t1 - t0).toFixed(0)}`);
                }).catch((err) => {
                    setPing("ERR");
                });
    }, []);
    return (
        <IonItem button onClick={(event) => {
            props.onClick(props.serverEntry);
        }}>
            <IonLabel>
                <img crossOrigin="anonymous" src={`https://flagcdn.com/w20/${props.serverEntry.geolocation === "" ? "gb" : props.serverEntry.geolocation.toLowerCase()}.png`} width="20"/>
                &nbsp;&nbsp;{props.serverEntry.serverIp}
            </IonLabel>
            <IonNote slot="end">
                {ping} ms
            </IonNote>
        </IonItem>
    );
}

export default ServerEntry;