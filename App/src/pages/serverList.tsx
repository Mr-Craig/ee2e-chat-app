import { IonApp, IonHeader, IonToolbar, IonTitle, IonContent, IonText, IonButtons, IonBackButton, IonSearchbar, IonItem, IonLabel, IonImg, IonRefresher, IonRefresherContent, RefresherEventDetail, useIonLoading, useIonAlert, IonPage } from "@ionic/react";
import { useEffect, useState } from "react";
import ServerEntry, { serverEntry } from "../components/serverEntry";
import Socket from "../helpers/socket";

const ServerList : React.FC = () => {
    const [serverList, setServerList] = useState([]);
    const [searchTerm, setSearchTerm] = useState("");

    const [present, dismiss] = useIonLoading();

    const [presentAlert] = useIonAlert();

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
        <IonPage>
            <IonHeader>
                <IonToolbar>
                    <IonTitle>Server List</IonTitle>
                    <IonButtons slot="start">
                        <IonBackButton defaultHref="/" />
                    </IonButtons>
                </IonToolbar>
                <IonToolbar>
                    <IonSearchbar animated={true} placeholder="Server Name" showClearButton="focus" onIonChange={(event) => {
                        setSearchTerm(event.detail.value as string);
                    }}></IonSearchbar>
                </IonToolbar>
            </IonHeader>
            <IonContent>
                <IonRefresher slot="fixed" pullFactor={0.5} pullMin={100} pullMax={200} onIonRefresh={handleRefresh}>
                    <IonRefresherContent></IonRefresherContent>
                </IonRefresher>
                
                {serverList.filter((server : serverEntry) => { return server.serverIp.search(searchTerm) != -1; } ).map((server : serverEntry, i) => {
                    return (
                        <ServerEntry key={i} serverEntry={server} onClick={(entry) => {
                            present({
                                message:"Connecting...",
                                spinner:"circles"
                            });
                            Socket.connect(entry.serverIp).then((res) => {
                                if(!res) {
                                    dismiss();
                                    presentAlert({
                                        header: 'Failed',
                                        message: 'Failed to connect to server, please try a different one.',
                                        buttons: ['OK']
                                    });
                                } else {
                                    dismiss();
                                    // TODO: ?
                                }
                            });
                        }}/>
                    )
                })}
            </IonContent>
        </IonPage>
    );
}

export default ServerList;