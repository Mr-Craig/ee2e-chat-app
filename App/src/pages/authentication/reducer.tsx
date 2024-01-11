import { ELoginAction, ILoginAction, ILoginState } from "./types";

export function authenticationReducder(state : ILoginState, action : ILoginAction) : ILoginState {
    const {act, data} = action;
    switch(act) {
        case ELoginAction.WEBAUTHN_SUPPORTED:
            return {
                ...state,
                webAuthSupported: data,
                registerDetails: {
                    ...state.registerDetails,
                    passwordless: data
                }
            };
        case ELoginAction.SEGMENT_CHANGE:
            return {
                ...state,
                segment: data
            };
        case ELoginAction.LOGIN_UPDATE:
            return {
                ...state,
                loginDetails: {
                    ...state.loginDetails,
                    ...data
                }
            };
        case ELoginAction.REGISTER_UPDATE:
            let updatedState = {
                ...state,
                registerDetails: {
                    ...state.registerDetails,
                    ...data
                }
            };
            return {
                ...state,
                registerDetails: {
                    ...state.registerDetails,
                    ...data,
                    passwordInvalid: updatedState.registerDetails.password.length >= 6,
                    confirmPasswordMatches: updatedState.registerDetails.password === updatedState.registerDetails.confirmPassword,
                    usernameError: updatedState.registerDetails.username.length >= 4 ? "Username is taken." : "Username must be atleast 4 characters",
                    validUsername: updatedState.registerDetails.username.length < 4 ? false : state.registerDetails.validUsername
                }
            }
        case ELoginAction.USERNAME_CHECK:
            return {
                ...state,
                registerDetails: {
                    ...state.registerDetails,
                    validUsername: data,
                    usernameError: "Username is Taken."
                }
            }
        case ELoginAction.REGISTER_BUTTON:
            return {
                ...state,
                registerDetails: {
                    ...state.registerDetails,
                    buttonDisabled: data
                }
            }
        default:
            return {
                ...state
            };
    }
}