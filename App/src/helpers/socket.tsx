import Store from "./store";

export enum SocketState {
    CONNECTED_AND_AUTHENTICATED,
    CONNECTED,
    DISCONNECTED
}

class SocketClass {
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

        this.on("authed", () => {
            this.socketState = SocketState.CONNECTED_AND_AUTHENTICATED;
            this.emit("socketState", this.socketState);
        });
    }

    // allows other processes to listen for events
    public on(event: string, cb : any)
    {
        if(typeof(this.callbacks[event]) === "undefined") {
            this.callbacks[event] = [];
        }

        this.callbacks[event].push(cb);
    }

    private emit(event: string, data : any)
    {
        let callbacks = this.callbacks[event];
        if(typeof(callbacks) !== "undefined") {
            callbacks.forEach(cb => cb(data));
        }
    }
    public connect(address : string) : Promise<boolean> {
        return new Promise((resolve, reject) => {
            this.socketAddress = address;
            this.socketObject = new WebSocket(`wss://${this.socketAddress}`);

            this.socketObject.addEventListener('open', (event) => {
                console.log("[SOCKET] Successfully Connected to "+this.socketAddress);   
                resolve(true);    
                
                this.socketState = SocketState.CONNECTED;
                this.emit("socketState", this.socketState);
                Store.set("socketAddress", this.socketAddress);
            });

            this.socketObject.addEventListener('close', (event) => {
                console.log("[SOCKET] Connection Closed: "+this.socketAddress);
                this.socketState = SocketState.DISCONNECTED;
                this.emit("socketState", this.socketState);
                Store.remove("socketAddress");
                resolve(false);
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

    private callbacks: { [s: string]: ((...args: any[]) => any)[] }
    private socketAddress : string = "";
    private socketState : SocketState = SocketState.DISCONNECTED;
    private socketObject : WebSocket | null = null;
}

const Socket = new SocketClass();

export default Socket;