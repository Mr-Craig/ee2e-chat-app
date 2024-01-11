import Store from "../../helpers/store"
import Socket from "../../helpers/socket";
import CurrentSettings from "../../helpers/settings";

export const promptAuthenticator = (registerUsername : any, data : any) : Promise<void> => {
    return new Promise(async (resolve, reject) => {
        let publicKeyCreationOptions = data;
        // https://stackoverflow.com/a/43131635
        publicKeyCreationOptions.challenge = new Uint8Array(publicKeyCreationOptions.challenge.match(/[\da-f]{2}/gi).map(function (h : any) {
            return parseInt(h, 16)
        }));
        publicKeyCreationOptions.user.id = Uint8Array.from(publicKeyCreationOptions.user.id, (c : any) => c.charCodeAt(0));
        await navigator.credentials.create({
            publicKey: publicKeyCreationOptions
        }).then(async (value) => { 
            CurrentSettings.addCredential(registerUsername, value?.id as string);
            let Response = (value as PublicKeyCredential).response as AuthenticatorAttestationResponse;
            Socket.send("finish_passwordless_register", {
                username: registerUsername,
                attestationObject: Array.from(new Uint8Array(Response.attestationObject)),
                clientDataJSON: Array.from(new Uint8Array(Response.clientDataJSON))
            });
        }).catch((err) => {
            reject("Unable to create credential.");
            console.log(err);
        });
    });
}

export const promptReAuthentication = (loginUsername : string, data : any) : Promise<void> => {
    return new Promise((resolve, reject) => {
        let publicKeyRequest : any = JSON.parse(data);
        publicKeyRequest.allowCredentials[0].id = new Uint8Array(publicKeyRequest.allowCredentials[0].id.match(/[\da-f]{2}/gi).map(function (h : any) {
            return parseInt(h, 16)
        }));
        publicKeyRequest.challenge = new Uint8Array(publicKeyRequest.challenge.match(/[\da-f]{2}/gi).map(function (h : any) {
            return parseInt(h, 16)
        }));
        navigator.credentials.get({
            publicKey: publicKeyRequest
        }).then((value) => {
            let publicKeyCredential = value as PublicKeyCredential;
            let assertationResponse = publicKeyCredential.response as AuthenticatorAssertionResponse;
            Socket.send("finish_login_passwordless", {
                username: loginUsername,
                authenticatorData: Array.from(new Uint8Array(assertationResponse.authenticatorData)),
                clientDataJSON: Array.from(new Uint8Array(assertationResponse.clientDataJSON)),
                signature: Array.from(new Uint8Array(assertationResponse.signature)),
                userHandle: Array.from(new Uint8Array(assertationResponse.userHandle as ArrayBuffer))
            });
        }).catch((err) => {
            console.log(err);
            reject(`Authenticator Login Unsuccessful: Error ${err}`);
        })
    })
}