import { IonApp, IonHeader, IonToolbar, IonTitle, IonContent, IonText, IonButtons, IonBackButton, IonSearchbar, IonItem, IonLabel, IonImg } from "@ionic/react";
import { useEffect, useState } from "react";

interface serverEntry 
{
    geolocation : string,
    serverIp : string
}

const ServerList : React.FC = () => {
    const [serverList, setServerList] = useState([]);
    const [searchTerm, setSearchTerm] = useState("");

    useEffect(() => {
        if(serverList.length <= 0) {
            console.log("Fetching Server List");
            fetch('https://lookup.acraig.tech/')
            .then((response) => response.json())
            .then((data) => setServerList(data));
        }
    }, []);

    useEffect(() => {
        //console.log(searchTerm.length);
    }, [searchTerm]);

    return (
        <IonApp>
            <IonHeader>
                <IonToolbar>
                    <IonTitle>Server List</IonTitle>
                    <IonButtons slot="start">
                        <IonBackButton defaultHref="/" />
                    </IonButtons>
                </IonToolbar>
            </IonHeader>
            <IonContent>
                <IonSearchbar animated={true} placeholder="Server Name" showClearButton="focus" onIonChange={(event) => {
                    setSearchTerm(event.detail.value as string);
                }}></IonSearchbar>
                {serverList.filter((server : serverEntry) => { return server.serverIp.search(searchTerm) != -1; } ).map((server : serverEntry, i) => {
                    return (
                        <IonItem button key={i}>
                            <IonLabel><img crossOrigin="anonymous" src={`https://countryflagsapi.com/png/${server.geolocation}`} height="11.2" width="16"/> {server.serverIp}</IonLabel>
                        </IonItem>
                    )
                })}
            </IonContent>
        </IonApp>
    );
}

export default ServerList;