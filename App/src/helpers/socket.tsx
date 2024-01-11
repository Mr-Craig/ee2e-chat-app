import { Handler } from "./handler";
import CurrentSettings from "./settings";
import Store from "./store";
import toastController from "./toastController";

export enum SocketState {
    CONNECTED_AND_AUTHENTICATED,
    CONNECTED,
    DISCONNECTED
}

export class SocketClass {
    constructor() {
        this.callbacks = {};

        Store.create().then((value) => {
            Store.get("socketAddress").then((value : string) => {
                if(value !== null) {
                    this.connect(value);
                } else {
                    console.log("[SOCKET] Old Connection was NULL");
                }
            });
        });

        this.on("authed", (data : any) => {
            this.socketState = SocketState.CONNECTED_AND_AUTHENTICATED;
            this.serverKey = data.key;
            this.emit("socketState", this.socketState);
            this.socketUsername = data.username;
            CurrentSettings.initUserData(this.socketUsername);
            CurrentSettings.setLastUsername(this.socketUsername);
           // console.log(data);
        });
    }
    public close() {
        Store.remove("socketAddress").then(() => {
            this.socketObject?.close();
        });
    }

    // allows other processes to listen for events
    public on(event: string, cb : any)
    {
        if(typeof(this.callbacks[event]) === "undefined") {
            this.callbacks[event] = [];
        }

        let newIndex = this.callbacks[event].push(cb) - 1;
        return newIndex;
    }

    public clearEvent(event : string, index?: number)
    {
        if(typeof(index) === "undefined") {
            // clear all callbacks
            delete this.callbacks[event];
        } else {
            // "delete" callback but don't effect index
            this.callbacks[event][index] = () => {};
        }
    }

    public send(event: string, body : any)
    {
        let object : object = {
            "event": event,
            "body": body
        }
        this.socketObject?.send(JSON.stringify(object));
    }

    public relay(username : string, event : string, body : any)
    {
        let relayObject = {
            "event": "relay",
            "body" : {
                "username": username,
                "body": {
                    "event": event,
                    "body": body
                }
            }
        }
        this.socketObject?.send(JSON.stringify(relayObject));
    }

    private emit(event: string, data : any)
    {
        let callbacks = this.callbacks[event];
        if(typeof(callbacks) !== "undefined") {
            callbacks.forEach(cb => cb(data));
        }
    }

    public getSocketUsername() : string { return this.socketUsername; };
    public getSocketState() : SocketState { return this.socketState; };
    public getSocketAddress() : string { return this.socketAddress; };
    public getServerKey() : string { return this.serverKey; };

    public connect(address : string) : Promise<boolean> {
        return new Promise((resolve, reject) => {
            this.socketAddress = address;
            this.socketObject = new WebSocket(`wss://${this.socketAddress}`);

            this.socketObject.addEventListener('open', (event) => {
                CurrentSettings.getSettings(this.socketAddress).then(() => {
                    console.log("[SOCKET] Successfully Connected to "+this.socketAddress);   
                    resolve(true);    
                    this.socketState = SocketState.CONNECTED;
                    this.emit("socketState", this.socketState);
                    Store.set("socketAddress", this.socketAddress);
                    Store.set("lastConnection", this.socketAddress);
                }).catch((err) => {
                    this.socketObject?.close();
                })
            });

            this.socketObject.addEventListener('close', (event) => {
                console.log("[SOCKET] Connection Closed: "+this.socketAddress);
                this.socketState = SocketState.DISCONNECTED;
                this.emit("socketState", this.socketState);
                Store.remove("socketAddress");
                resolve(false);
                toastController.sendToast({
                    message: "You were disconnected from the server.",
                    color: "danger",
                    duration: 2000
                })
            });

            this.socketObject.addEventListener('message', (event) => {
                try {
                    let messageJson = JSON.parse(event.data);

                    if(typeof(messageJson["event"]) !== "undefined" && typeof(messageJson["body"]) !== "undefined") {
                        this.emit(messageJson.event, messageJson.body);
                    } else {
                        console.error("[SOCKET] Message was JSON but formatted incorrectly.");
                    }
                } catch {
                    console.error("[SOCKET] Message recieved wasn't JSON.");
                }
            });
        });
    }

    private socketUsername : string = "";
    private callbacks: { [s: string]: ((...args: any[]) => any)[] }
    private socketAddress : string = "";
    private serverKey : string = "";
    private socketState : SocketState = SocketState.DISCONNECTED;
    private socketObject : WebSocket | null = null;
}

const Socket = new SocketClass();

const handleMessages = new Handler(Socket);
export default Socket;