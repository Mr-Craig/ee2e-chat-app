import { Redirect, Route } from "react-router";
import React, { useContext, useEffect, useState } from "react";
import {} from "react-router-dom";

import Socket, { SocketState } from "../helpers/socket";
import { IonText, useIonViewDidEnter, useIonViewDidLeave, useIonViewWillEnter } from "@ionic/react";

interface IProtectedRoute
{
    path : string,
    redirectPath : string,
    needsState : SocketState,
    children? : React.ReactNode
}

const ProtectedRoute : React.FC<IProtectedRoute> = (props, {children}) => {
    const [shouldShow, setShouldShow] = useState<boolean>(Socket.getSocketState() === props.needsState);
    const [isListening, setIsListening] = useState<boolean>(false);
    useEffect(() => {
        if(!isListening) {
            // state could be what we need before listening!
            setIsListening(true);
            Socket.on("socketState", (newState : SocketState) => {
                console.log(`[TEXT] ${props.path} -> ${newState === props.needsState}`);
                setShouldShow(newState === props.needsState);
            });
        }
    }, []);
    return (
        <Route path={props.path} exact={true}>
            {shouldShow ? props.children 
            : isListening ? (
                <Redirect to={props.redirectPath}/>
            ) : (<div></div>)}
        </Route>
    )
}

export default ProtectedRoute;