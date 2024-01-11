import { IonApp, IonHeader, IonToolbar, IonNote, IonTitle, IonContent, IonButtons, IonItem, IonLabel, IonIcon, IonGrid, IonRow, IonButton, IonInput, IonSegment, IonSegmentButton, IonList, IonCheckbox, useIonLoading, useIonAlert, isPlatform, useIonViewDidEnter } from "@ionic/react";
import { useCallback, useEffect, useReducer } from "react";
import { arrowBack } from "ionicons/icons";

import Socket from "../../helpers/socket";
import { promptAuthenticator, promptReAuthentication } from "./helpers";
import CurrentSettings from "../../helpers/settings";
import { hideTabBar } from "../../App";
import toastController from "../../helpers/toastController";
import { ELoginAction, LoginStateDefault } from "./types";
import { authenticationReducder } from "./reducer";
import { useSocket } from "../../helpers/socketHook";

const LoginPage : React.FC = () => {
    const [presentLoading, dismiss] = useIonLoading();
    const [presentAlert] = useIonAlert();
    const [state, dispatch] = useReducer(authenticationReducder, {
        ...LoginStateDefault,
        loginDetails: {
            ...LoginStateDefault.loginDetails,
            username: CurrentSettings.getLastUsername(),
            foundCredential: CurrentSettings.hasCredential(CurrentSettings.getLastUsername())
        }
    });

    useEffect(() => {
        // check if webauthn is support in browser
        if(window.PublicKeyCredential) {
            window.PublicKeyCredential.isUserVerifyingPlatformAuthenticatorAvailable().then((available) => {
                console.log(`Platform Authenticator: ${available}`);
                dispatch({act: ELoginAction.WEBAUTHN_SUPPORTED, data: available});
            });
        } else {
            dispatch({act: ELoginAction.WEBAUTHN_SUPPORTED, data: false});
        }
    }, []);

    useIonViewDidEnter(() => {
        hideTabBar();
    });

    useEffect(() => {
        if(state.registerDetails.username.length >= 4) Socket.send("username_check", state.registerDetails.username);
    }, [state.registerDetails.username]);

    const register = useCallback(() => {
        dispatch({act: ELoginAction.REGISTER_BUTTON, data: true});
        if(state.registerDetails.passwordless) {
            Socket.send("begin_passwordless_register", {
                username: state.registerDetails.username
            });
        } else {
            Socket.send("register", {
                "username": state.registerDetails.username,
                "password": state.registerDetails.password
            });
        }
    }, [state]);

    const login = useCallback(() => {
        if(state.loginDetails.foundCredential) {
            presentLoading({
                message: "Authenticating..."
            });
            Socket.send("begin_login_passwordless", {
                "credentialId": CurrentSettings.getCredential(state.loginDetails.username)
            });
        } else {
            Socket.send("login", state.loginDetails);
        }
    }, [state, presentLoading]);

    useSocket("begin_passwordless_register", (data : any) => {
        let jsonData = JSON.parse(data);
        jsonData.rp.id = window.location.hostname;
        promptAuthenticator(state.registerDetails.username, jsonData).catch((err) => {
            toastController.sendToast({
                message: `${err}`,
                duration: 1500,
                color: "danger"
            });
            dispatch({act: ELoginAction.REGISTER_BUTTON, data: false});
        });
    }, [state]);

    useSocket("begin_login_passwordless", (data: any) => {
        // Safari requires user input to open the authenticator prompt (!)
        if(isPlatform("ios")) {
            dismiss();
            presentAlert("Press OK to open authenticator.", [{
                text:"OK",
                handler: () => {
                    presentLoading({
                        message: "Authenticating..."
                    });
                    promptReAuthentication(state.loginDetails.username, data).catch((err) => {
                        toastController.sendToast({
                            message: `${err}`,
                            duration: 1500,
                            color: "danger"
                        });
                        dismiss();
                    })
                }
            }]);
        } else {
            promptReAuthentication(state.loginDetails.username, data).catch((err) => {
                toastController.sendToast({
                    message: `${err}`,
                    duration: 1500,
                    color: "danger"
                });
                dismiss();
            })
        }
    }, [state.loginDetails.username]);

    useSocket("login", (data : any) => {
        dismiss();
        if(data.success) {
            CurrentSettings.setLastUsername(state.loginDetails.username);
            toastController.sendToast({
                message: "Login success, redirecting...",
                duration: 1000,
                color: "success"
              });
        } else {
            toastController.sendToast({
                message: `Login Failed: ${data.message}`,
                duration: 2000,
                color: "danger"
              });
        }
    }, [state]);

    useSocket("register", (data : any) => {
        if(data.success) {
            CurrentSettings.setLastUsername(state.registerDetails.username);
            toastController.sendToast({
                message: "Register success, redirecting...",
                duration: 1000,
                color: "success"
              });
        } else {
            toastController.sendToast({
                message: `Register Failed: ${data.message}`,
                duration: 3000,
                color: "danger"
            });
        }
        dispatch({act: ELoginAction.REGISTER_BUTTON, data: false});
    }, [state]); 

    useSocket("username_check", (data: {username: string, available:boolean}) => {
        if(data.username === state.registerDetails.username) dispatch({act: ELoginAction.USERNAME_CHECK, data: data.available});
    }, [state]);

    return (
        <IonApp>
            <IonHeader>
                <IonToolbar>
                    <IonTitle>Authentication</IonTitle>
                    <IonButtons slot="start">
                        <IonButton onClick={(event) => {
                            Socket.close();
                        }}>
                            <IonIcon icon={arrowBack}></IonIcon>
                        </IonButton>
                    </IonButtons>
                </IonToolbar>
                <IonToolbar>
                    <IonSegment value={state.segment} onIonChange={(event) => dispatch({act: ELoginAction.SEGMENT_CHANGE,data: event.target.value})}>
                        <IonSegmentButton value="login">
                            Login
                        </IonSegmentButton>
                        <IonSegmentButton value="register">
                            Register
                        </IonSegmentButton>
                    </IonSegment>
                </IonToolbar>
            </IonHeader>
            <IonContent>
                <IonGrid>
                {state.segment === "login" ? (
                    <IonRow>
                        <IonList className="ion-padding" style={{
                            width: "100%"
                        }}>
                            <IonItem className="ion-no-padding" lines="full" style={{
                                background: "none"
                            }}>
                                <IonLabel position="floating">Username</IonLabel>
                                <IonInput onIonInput={(event) => dispatch({act: ELoginAction.LOGIN_UPDATE, data: { username: event.target.value }})} value={state.loginDetails.username}></IonInput>
                            </IonItem>
                            {!state.loginDetails.foundCredential && (
                                <IonItem className="ion-no-padding" lines="full" style={{
                                    background: "none"
                                }}>
                                    <IonLabel position="floating">Password</IonLabel>
                                    <IonInput type="password" onIonInput={(event) => dispatch({act: ELoginAction.LOGIN_UPDATE, data: { password: event.target.value }})} value={state.loginDetails.password}></IonInput>
                                </IonItem>
                            )}
                            <IonButton expand="full" style={{
                                marginTop: "25px"
                            }} onClick={login}>
                                Login
                            </IonButton>
                        </IonList>
                    </IonRow>
                ) : (
                    <IonRow>
                        <IonList className="ion-padding" style={{
                            width: "100%"
                        }}>
                            <IonItem className={`${state.registerDetails.validUsername && 'ion-valid'} ${state.registerDetails.validUsername === false && 'ion-invalid'} ion-no-padding`} lines="full" style={{
                                background: "none"
                            }}>
                                <IonLabel position="floating">Username</IonLabel>
                                <IonInput onIonInput={(event) => dispatch({act: ELoginAction.REGISTER_UPDATE, data: {username: event.target.value}})} value={state.registerDetails.username}></IonInput>
                                <IonNote slot="error">{state.registerDetails.usernameError}</IonNote>
                            </IonItem>
                            {!state.registerDetails.passwordless && (
                                <IonItem className={`${state.registerDetails.passwordInvalid === true && 'ion-valid'} ${state.registerDetails.passwordInvalid === false && 'ion-invalid'} ion-no-padding`} lines="full" style={{
                                    background: "none"
                                }}>
                                    <IonLabel position="floating">Password</IonLabel>
                                    <IonInput type="password" onIonInput={(event) => dispatch({act: ELoginAction.REGISTER_UPDATE, data: {password: event.target.value}})} value={state.registerDetails.password}></IonInput>
                                    <IonNote slot="error">Password must be atleast 6 characters</IonNote>
                                </IonItem>
                            )}
                            {!state.registerDetails.passwordless && (
                                <IonItem className={`${state.registerDetails.confirmPasswordMatches && 'ion-valid'} ${state.registerDetails.confirmPasswordMatches === false && 'ion-invalid'} ion-no-padding`} lines="full" style={{
                                    background: "none"
                                }}>
                                    <IonLabel position="floating">Confirm Password</IonLabel>
                                    <IonInput type="password" onIonInput={(event) => dispatch({act: ELoginAction.REGISTER_UPDATE, data: {confirmPassword: event.target.value}})} value={state.registerDetails.confirmPassword}></IonInput>
                                    <IonNote slot="error">Password doesn't match</IonNote>
                                </IonItem>
                            )}
                            <IonItem className={`ion-no-padding`} lines="full" style={{
                                background: "none"
                            }}>
                                <IonCheckbox slot="start" checked={state.registerDetails.passwordless} disabled={!state.webAuthSupported} onIonChange={(event) => dispatch({act: ELoginAction.REGISTER_UPDATE, data: {passwordless: event.target.checked}})}></IonCheckbox>
                                <IonLabel>Use Authenticator</IonLabel>
                            </IonItem>
                            <IonButton expand="full" style={{
                                marginTop: "25px"
                            }} disabled={state.registerDetails.buttonDisabled} onClick={register}>
                                Register
                            </IonButton>
                        </IonList>
                    </IonRow>
                )}
                </IonGrid>
            </IonContent>
        </IonApp>
    );
}

export default LoginPage;