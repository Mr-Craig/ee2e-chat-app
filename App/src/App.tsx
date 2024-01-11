import { IonApp, IonBadge, IonHeader, IonIcon, IonLabel, IonRouterOutlet, IonSplitPane, IonTabBar, IonTabButton, IonTabs, IonText, setupIonicReact, useIonToast, useIonViewDidEnter } from '@ionic/react';
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
import React, { useEffect, useReducer, useState } from 'react';
import Socket, { SocketState } from './helpers/socket';
import { chatbubble, cog, handRightSharp, peopleCircle } from 'ionicons/icons';
import ProtectedRoute from './components/protectedRoute';
import ChatsPage from './pages/chats/chats';
import ContactsPage from './pages/contacts/contacts';
import SettingsPage from './pages/settings/settings';
import toastController from './helpers/toastController';
import ChatPage from './pages/chat/chat';
import VerifyChatPage from './pages/chat/verify';
import CurrentSettings from './helpers/settings';
setupIonicReact();

export interface IAppContext
{
  State : SocketState
}

export const showTabBar = (): void => {
  const tabBar = document.getElementById('app-tab-bar');
  if (tabBar !== null) {
    tabBar.style.display = 'flex';
  }
};

export const hideTabBar = (): void => {
  const tabBar = document.getElementById('app-tab-bar');
  if (tabBar !== null) {
    tabBar.style.display = 'none';
  }
};

const App: React.FC = () => {
  const [present, dismiss] = useIonToast();
  const [pendingRequests, setPendingRequests] = useState(0);

  toastController.setControllerCallback(async (newToast) => {
    await dismiss();
    await present(newToast);
  });
   
  CurrentSettings.setPendingContactsCallback(() => {
    setPendingRequests(CurrentSettings.getPendingContacts());
  });

  useEffect(() => {
    if ('serviceWorker' in navigator) {
      navigator.serviceWorker.onmessage = (event) => {
        if(event.data.type && event.data.type === 'NOTIF_OFF') {
          // notifs OFF
          toastController.sendToast({
            message: event.data.error ? "Failed to unsubscribe, maybe you were never subscribed!" : "Successfully unsubcribed from notifications",
            color: 'success',
            duration: 1500
          });
        } else if(event.data.type && event.data.type === 'NOTIF_ON') {
          if(event.data.error) {
            toastController.sendToast({
              message: "Failed to subscribe to notifications, could be an issue with the server.",
              color: 'danger',
              duration: 1500
            });
            CurrentSettings.setNotificationsValue(false);
          } else {
            toastController.sendToast({
              message: "Successfully subscribed to notifications, you will recieve messages even when offline!",
              color: 'success',
              duration: 1500
            });

            Socket.send("notifications", {
              sub: event.data.sub
            });
            CurrentSettings.setNotificationsValue(true);
          }
        }
      }
    }
  }, []);
  
  return (
    <IonApp>
      <IonReactRouter>
        <IonTabs>
          <IonRouterOutlet animated={false}> 
              <ProtectedRoute path="/servers" needsState={SocketState.DISCONNECTED} redirectPath="/login">
                <ServerList/>
              </ProtectedRoute>
              <ProtectedRoute path="/splash" needsState={SocketState.DISCONNECTED} redirectPath="/login">
                <SplashPage/>
              </ProtectedRoute>
              <ProtectedRoute path="/login" needsState={SocketState.CONNECTED} redirectPath="/chats">
                <LoginPage/>
              </ProtectedRoute>
              <ProtectedRoute path="/chats" needsState={SocketState.CONNECTED_AND_AUTHENTICATED} redirectPath="/splash">
                <ChatsPage/>
              </ProtectedRoute>
              <ProtectedRoute path="/contacts" needsState={SocketState.CONNECTED_AND_AUTHENTICATED} redirectPath="/splash">
                <ContactsPage/>
              </ProtectedRoute>
              <ProtectedRoute path="/settings" needsState={SocketState.CONNECTED_AND_AUTHENTICATED} redirectPath="/splash">
                <SettingsPage/>
              </ProtectedRoute>
              <ProtectedRoute path="/chatverify/:username" needsState={SocketState.CONNECTED_AND_AUTHENTICATED} redirectPath="/splash">
                <VerifyChatPage/>
              </ProtectedRoute>
              <ProtectedRoute path="/chat/:username" needsState={SocketState.CONNECTED_AND_AUTHENTICATED} redirectPath="/splash">
                <ChatPage/>
              </ProtectedRoute>
              <Route path="/" exact>
                <Redirect to="/splash"/>
              </Route>
          </IonRouterOutlet>
          <IonTabBar slot="bottom" id="app-tab-bar" style={{
            display: "none"
          }}>
            <IonTabButton tab="chats" href="/chats">
              <IonIcon icon={chatbubble}/>
              <IonLabel>Chats</IonLabel>
            </IonTabButton>
            <IonTabButton tab="contacts" href="/contacts">
              <IonIcon icon={peopleCircle}/>
              <IonBadge>{pendingRequests}</IonBadge>
              <IonLabel>Contacts</IonLabel>
            </IonTabButton>
            <IonTabButton tab="settings" href="/settings">
              <IonIcon icon={cog}/>
              <IonLabel>Settings</IonLabel>
            </IonTabButton>
        </IonTabBar>
        </IonTabs>
      </IonReactRouter>
    </IonApp>
  );
};

export default App;
