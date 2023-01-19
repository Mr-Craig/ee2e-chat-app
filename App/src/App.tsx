import { IonApp, IonHeader, IonLabel, IonRouterOutlet, IonSplitPane, IonTabBar, IonTabButton, IonTabs, IonText, setupIonicReact, useIonToast } from '@ionic/react';
import { IonReactRouter } from '@ionic/react-router';
import { Link, Redirect, Route, useHistory } from 'react-router-dom';

/* Core CSS required for Ionic components to work properly */
import '@ionic/react/css/core.css';

/* Basic CSS for apps built with Ionic */
import '@ionic/react/css/normalize.css';
import '@ionic/react/css/structure.css';
import '@ionic/react/css/typography.css';

/* Optional CSS utils that can be commented out */
import '@ionic/react/css/padding.css';
import '@ionic/react/css/float-elements.css';
import '@ionic/react/css/text-alignment.css';
import '@ionic/react/css/text-transformation.css';
import '@ionic/react/css/flex-utils.css';
import '@ionic/react/css/display.css';

/* Theme variables */
import './theme/variables.css';

// Pages
import ServerList from './pages/serverList';
import SplashPage from './pages/splash/splash';
import LoginPage from './pages/authentication/authentication';
import Store from './helpers/store';
import { useEffect, useReducer, useState } from 'react';
import Socket, { SocketState } from './helpers/socket';
import { handRightSharp } from 'ionicons/icons';

setupIonicReact();

const App: React.FC = () => {
  const [state, setState] = useState<SocketState>(SocketState.DISCONNECTED);
  const [ignored, forceUpdate] = useReducer(x => x + 1, 0);
 
  const [present] = useIonToast();

  useEffect(() => {
    Socket.clearEvent("socketState");
    Socket.on("socketState", (newState : SocketState) => {
      if(state !== SocketState.DISCONNECTED && newState === SocketState.DISCONNECTED) {
        present({
          message: "You were disconnected from the server.",
          duration: 3000,
          color: "danger"
        });
      }
      if(newState !== state) {
        setState(newState);
        forceUpdate();
        console.log("[MAIN] State Change");
      }
    });
  }, [state, setState, forceUpdate]);

  return (
    <IonApp>
      <IonReactRouter>
        <IonTabs>
          <IonRouterOutlet animated={true}>
            <Route path="/servers" exact={true} render={(props) => {
                if(state === SocketState.DISCONNECTED) {
                  return (<ServerList/>);
                } else {
                  console.log("[TEST] Redirecting to /login");
                  return (<Redirect to="/login"/>);
                }
            }}/>
            <Route path="/splash" exact={true} render={(props) => {
                if(state === SocketState.DISCONNECTED) {
                  return (<SplashPage/>);
                } else {
                  console.log("[TEST] Redirecting to /login");
                  return (<Redirect to="/login"/>);
                }
            }}/>
            <Route path="/login" exact={true} render={(props) => {
                if(state !== SocketState.DISCONNECTED) {
                  if(state === SocketState.CONNECTED_AND_AUTHENTICATED) {
                    return (<Redirect to="/main"/>);
                  } else {
                    return (<LoginPage/>);
                  }
                } else {
                  return (<Redirect to="/splash"/>);
                }
            }}/>
            <Route path="/main" exact={true} render={(props) => {
              if(state === SocketState.CONNECTED_AND_AUTHENTICATED) {
                return (<IonText>Authed</IonText>);
              } else {
                console.log("[TEST] Redirecting to /splash");
                return (<Redirect to="/splash"/>);
              }
            }}/>
            <Route render={(props) => {
              console.log("[TEST] Redirecting to /splash");
              return (<Redirect to="/splash"/>);
            }}/>
          </IonRouterOutlet>
          <IonTabBar slot="bottom" style={{
            display: "none"
          }}>
            <IonTabButton tab="chats" href="/chats">
              <IonLabel>Chats</IonLabel>
            </IonTabButton>
            <IonTabButton tab="contacts" href="/contacts">
              <IonLabel>Contacts</IonLabel>
            </IonTabButton>
            <IonTabButton tab="settings" href="/settings">
              <IonLabel>Settings</IonLabel>
            </IonTabButton>
        </IonTabBar>
        </IonTabs>
      </IonReactRouter>
    </IonApp>
  );
};

export default App;
