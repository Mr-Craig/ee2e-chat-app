import { ToastOptions } from "@ionic/core";

// TODO: Implement queue as IonToast doesn't stack toasts.
class ToastController
{
    constructor() {
    }

    public sendToast(n : ToastOptions) { this.controllerCallback !== null ? this.controllerCallback(n) : console.log("no cb set"); };
    public setControllerCallback(newCb : (n: ToastOptions) => any) { this.controllerCallback = newCb; }; 
    
    private controllerCallback : ((n: ToastOptions) => any) | null = null;
}

const toastController = new ToastController();

export default toastController;