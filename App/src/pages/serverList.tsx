import { IonApp, IonHeader, IonToolbar, IonTitle, IonContent, IonText, IonButtons, IonBackButton, IonSearchbar, IonItem, IonLabel, IonImg, IonRefresher, IonRefresherContent, RefresherEventDetail } from "@ionic/react";
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
            .then((data) => setServerList(data))
            .catch((err) => {
                console.log(err);
            });
        }
    }, []);

    useEffect(() => {
        //console.log(searchTerm.length);
    }, [searchTerm]);

    function handleRefresh(event: CustomEvent<RefresherEventDetail>) {
        fetch('https://lookup.acraig.tech/')
            .then((response) => response.json())
            .then((data) => {
                setServerList(data);
                event.detail.complete();
            }).catch((err) => {
                console.log(err);
            });
    }

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
                <IonRefresher slot="fixed" pullFactor={0.5} pullMin={100} pullMax={200} onIonRefresh={handleRefresh}>
                    <IonRefresherContent></IonRefresherContent>
                </IonRefresher>
                <IonSearchbar animated={true} placeholder="Server Name" showClearButton="focus" onIonChange={(event) => {
                    setSearchTerm(event.detail.value as string);
                }}></IonSearchbar>
                {serverList.filter((server : serverEntry) => { return server.serverIp.search(searchTerm) != -1; } ).map((server : serverEntry, i) => {
                    return (
                        <IonItem button key={i}>
                            <IonLabel><img crossOrigin="anonymous" src={`https://flagcdn.com/w20/${server.geolocation === "" ? "gb" : server.geolocation.toLowerCase()}.png`} width="20"/> {server.serverIp}</IonLabel>
                        </IonItem>
                    )
                })}
            </IonContent>
        </IonApp>
    );
}

export default ServerList;