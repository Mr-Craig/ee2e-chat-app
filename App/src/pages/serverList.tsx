import { IonApp, IonHeader, IonToolbar, IonTitle, IonContent, IonText, IonButtons, IonBackButton, IonSearchbar, IonItem, IonLabel, IonImg, IonRefresher, IonRefresherContent, RefresherEventDetail, useIonLoading, useIonAlert, IonPage, useIonViewDidEnter } from "@ionic/react";
import { useCallback, useEffect, useMemo, useState } from "react";
import { hideTabBar } from "../App";
import ServerEntry, { serverEntry } from "../components/serverEntry";
import Socket from "../helpers/socket";
import toastController from "../helpers/toastController";

const ServerList : React.FC = () => {
    const [serverList, setServerList] = useState([]);
    const [searchTerm, setSearchTerm] = useState("");

    const [present, dismiss] = useIonLoading();

    const [presentAlert] = useIonAlert();

    const refreshServers = useCallback(() => {
        return new Promise<void>((resolve, reject) => fetch('https://lookup.acraig.tech/')
            .then((response) => response.json())
            .then((data) => {
                setServerList(data);
                resolve();
            })
            .catch((err) => {
                console.error(err);
                toastController.sendToast({
                    message: "Error getting servers",
                    color: "danger",
                    duration: 1000
                });
                reject();
        }));
    }, [setServerList]);

    useEffect(() => {
        if(serverList.length <= 0) refreshServers();
    }, []);

    const filteredServers = useMemo(() => serverList.filter((server : serverEntry) => { return server.serverIp.search(searchTerm) != -1; } ), [serverList, searchTerm]);

    useIonViewDidEnter(() => {
        hideTabBar();
    });

    async function handleRefresh(event: CustomEvent<RefresherEventDetail>) {
        await refreshServers().catch((err) => console.error(err));
        event.detail.complete();
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
                
                {filteredServers.map((server : serverEntry, i) => {
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