import { IonApp, IonHeader, IonRouterOutlet, IonSplitPane, IonText, setupIonicReact, useIonToast } from '@ionic/react';
import { IonReactRouter } from '@ionic/react-router';
import { Link, Redirect, Route } from 'react-router-dom';

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
import LoginPage from './pages/authentication/login';
import Store from './helpers/store';
import { useState } from 'react';
import Socket, { SocketState } from './helpers/socket';

setupIonicReact();

const App: React.FC = () => {
  const [connected, setConnected] = useState(false);
  const [authed, setAuthed] = useState(false);
 
  const [present] = useIonToast();
  Socket.on("socketState", (newState : SocketState) => {
    if(connected && newState == SocketState.DISCONNECTED) {
      present({
        message: "You were disconnected from the server.",
        duration: 3000,
        color: "danger"
      })
    }
    setAuthed(newState == SocketState.CONNECTED_AND_AUTHENTICATED);
    setConnected(newState == SocketState.CONNECTED);
  });
  return (
    <IonApp>
      <IonReactRouter>
        <IonSplitPane contentId="main">
          <IonRouterOutlet id="main">
            <Route path="/servers" exact={true} render={(props) => {
                if(!connected) {
                  return (<ServerList/>);
                }
                return (<Redirect to="/login"/>);
            }}/>
            <Route path="/" exact={true} render={(props) => {
                if(!connected) {
                  return (<SplashPage/>);
                }
                return (<Redirect to="/login"/>);
            }}/>
            <Route path="/login" exact={true} render={(props) => {
                if(connected && !authed) {
                  return (<LoginPage/>);
                }
                return (<Redirect to="/"/>);
            }}/>
          </IonRouterOutlet>
        </IonSplitPane>
      </IonReactRouter>
    </IonApp>
  );
};

export default App;
