import CurrentSettings from "../settings";
import { EChatState, EMessageState, IChat, IMessage } from "../types";

export const messageStateEnumToString = (state : EMessageState) => {
    switch(state) {
      case EMessageState.DELIVERED:
          return "Delivered";
      case EMessageState.READ:
          return "Read";
      case EMessageState.SENT:
          return "Sent";
      default:
          return "-";
    }  
  };

export default class Chat
{
    constructor(initData : IChat) {
        this.data = initData;
    }

    public setKeys(newKeys : {publicKey:string, privateKey:string}) {
        this.update();
        this.data.ecKey = newKeys;
        this.save();
    }
    public setState(newState : EChatState) {
        this.update();
        this.data.state = newState;
        this.save();
    }
    public setSharedSecret(newSecret : string) {
        this.update();
        this.data.sharedSecret = newSecret;
        this.save();
    }

    public getKeys() : {publicKey:string, privateKey:string} {
        this.update();
        if(typeof(this.data.ecKey) === "undefined") {
            return  {
                publicKey: "",
                privateKey: ""
            };
        }
        return this.data.ecKey;
    }
    public getState() : EChatState { 
        this.update();
        return this.data.state; 
    };
    public getUsername() : string { 
        this.update();
        return this.data.username; 
    };
    public getSharedSecret() : string { 
        this.update();
        return typeof(this.data.sharedSecret) === "undefined" ? "" : this.data.sharedSecret; 
    };

    public addMessage(id: string, newMessage : IMessage) {
        this.update();
        this.data.messages[id] = newMessage;
        this.save();
    }
    public getMessages() : IMessage[] {
        this.update();
        return Object.values(this.data.messages);
    }

    public setMessageState(id: string, newState : EMessageState) {
        this.update();
        this.data.messages[id].state = newState;
        this.save();
    }
    // always get updated information before passing information.
    public update() {
        this.data = CurrentSettings.getChatData(this.data.username);
    }

    private save() {
        CurrentSettings.updateChat(this.data.username, this.data);
    }

    private data : IChat;
}