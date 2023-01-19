import { IonApp, IonHeader, IonToolbar, IonNote, IonTitle, IonContent, IonText, IonButtons, IonBackButton, IonSearchbar, IonItem, IonLabel, IonImg, IonIcon, IonGrid, IonRow, IonCol, IonButton, IonRippleEffect, IonInput, IonSegment, IonSegmentButton, IonList, useIonToast } from "@ionic/react";
import { useEffect, useState } from "react";
import { chatbubbles, arrowBack } from "ionicons/icons";

import { Link, useHistory } from "react-router-dom";
import Socket from "../../helpers/socket";

const LoginPage : React.FC = () => {
    const [segment, setSegment] = useState("login");

    const [present] = useIonToast();
    // gotta be a better way of doing this
    const [loginUsername, setLoginUsername] = useState("");
    const [loginPassword, setLoginPassword] = useState("");

    const [registerUsername, setRegisterUsername] = useState("");
    const [registerPassword, setRegisterPassword] = useState("");
    const [registerConfirmPassword, setRegisterConfirmPassword] = useState("");
    const [registerValidUsername, setIsValidUsername] = useState(true);
    const [registerUsernameError, setRegisterUsernameError] = useState("");
    const [registerPasswordInvalid, setRegisterPasswordInvalid] = useState(false);
    const [registerConfirmPasswordMatches, setRegisterConfirmPasswordMatches] = useState(false);

    useEffect(() => {
        Socket.clearEvent("login");
        Socket.clearEvent("register");
        Socket.on("login", (data : any) => {
            if(data.success) {
                present({
                    message: "Login success, redirecting...",
                    duration: 2000,
                    color: "success"
                  });
            } else {
                present({
                    message: `Login Failed: ${data.message}`,
                    duration: 3000,
                    color: "danger"
                  });
            }
        });
        Socket.on("register", (data : any) => {
            if(data.success) {
                present({
                    message: "Register success, redirecting...",
                    duration: 2000,
                    color: "success"
                  });
            } else {
                present({
                    message: `Register Failed: ${data.message}`,
                    duration: 3000,
                    color: "danger"
                  });
            }
        });
    }, []);
    // check if valid
    useEffect(() => {
        Socket.clearEvent("username_check");
        Socket.on("username_check", (data : any) => {
            if(data.username === registerUsername) {
                setIsValidUsername(data.available as boolean);
            }
        });
    }, [registerUsername]);

    useEffect(() => {
        console.log(registerPassword.length < 6);
        setRegisterPasswordInvalid(registerPassword.length >= 6);
    }, [registerPassword]);

    useEffect(() => {
        setRegisterConfirmPasswordMatches(registerConfirmPassword == registerPassword);
    }, [registerConfirmPassword,registerPassword]);

    useEffect(() => {
        if(registerUsername.length >= 4) {
            Socket.send("username_check", registerUsername);
            setRegisterUsernameError("Username is taken.");
        } else {
            setRegisterUsernameError("Username must be atleast 4 characters.");
            setIsValidUsername(false);
        }
    }, [registerUsername]);
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
                    <IonSegment value={segment} onIonChange={(event) => {
                        setSegment(event.detail.value as string);
                    }}>
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
                {segment === "login" ? (
                    <IonRow>
                        <IonList className="ion-padding" style={{
                            width: "100%"
                        }}>
                            <IonItem className="ion-no-padding" lines="full" style={{
                                background: "none"
                            }}>
                                <IonLabel position="floating">Username</IonLabel>
                                <IonInput onIonInput={(event) => setLoginUsername(event.target.value as string)} value={loginUsername}></IonInput>
                            </IonItem>
                            <IonItem className="ion-no-padding" lines="full" style={{
                                background: "none"
                            }}>
                                <IonLabel position="floating">Password</IonLabel>
                                <IonInput type="password" onIonInput={(event) => setLoginPassword(event.target.value as string)} value={loginPassword}></IonInput>
                            </IonItem>
                            <IonButton expand="full" style={{
                                marginTop: "25px"
                            }} onClick={(event) => {
                                Socket.send("login", {
                                    "username": loginUsername,
                                    "password": loginPassword
                                });
                            }}>
                                Login
                            </IonButton>
                        </IonList>
                    </IonRow>
                ) : (
                    <IonRow>
                        <IonList className="ion-padding" style={{
                            width: "100%"
                        }}>
                            <IonItem className={`${registerValidUsername && 'ion-valid'} ${registerValidUsername === false && 'ion-invalid'} ion-no-padding`} lines="full" style={{
                                background: "none"
                            }}>
                                <IonLabel position="floating">Username</IonLabel>
                                <IonInput onIonInput={(event) => setRegisterUsername(event.target.value as string)} value={registerUsername}></IonInput>
                                <IonNote slot="error">{registerUsernameError}</IonNote>
                            </IonItem>
                            <IonItem className={`${registerPasswordInvalid === true && 'ion-valid'} ${registerPasswordInvalid === false && 'ion-invalid'} ion-no-padding`} lines="full" style={{
                                background: "none"
                            }}>
                                <IonLabel position="floating">Password</IonLabel>
                                <IonInput type="password" onIonInput={(event) => setRegisterPassword(event.target.value as string)} value={registerPassword}></IonInput>
                                <IonNote slot="error">Password must be atleast 6 characters</IonNote>
                            </IonItem>
                            <IonItem className={`${registerConfirmPasswordMatches && 'ion-valid'} ${registerConfirmPasswordMatches === false && 'ion-invalid'} ion-no-padding`} lines="full" style={{
                                background: "none"
                            }}>
                                <IonLabel position="floating">Confirm Password</IonLabel>
                                <IonInput type="password" onIonInput={(event) => setRegisterConfirmPassword(event.target.value as string)} value={registerConfirmPassword}></IonInput>
                                <IonNote slot="error">Password do not match</IonNote>
                            </IonItem>
                            <IonButton expand="full" style={{
                                marginTop: "25px"
                            }} onClick={(event) => {
                                Socket.send("register", {
                                    "username": registerUsername,
                                    "password": registerPassword
                                });
                            }}>
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