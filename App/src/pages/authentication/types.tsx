export enum ELoginAction
{
    WEBAUTHN_SUPPORTED,
    SEGMENT_CHANGE,
    LOGIN_UPDATE,
    REGISTER_UPDATE,
    REGISTER_BUTTON,
    USERNAME_CHECK
}

export interface ILoginAction
{
    act: ELoginAction,
    data: any
}
export interface ILoginDetails
{
    username: string, 
    password: string,
    foundCredential: boolean
}

export interface IRegisterDetails
{
    username : string, 
    password: string, 
    confirmPassword: string, 
    validUsername : boolean, 
    usernameError : string, 
    passwordInvalid : boolean, 
    confirmPasswordMatches : boolean,
    passwordless : boolean,
    buttonDisabled: boolean
}

export interface ILoginState
{
    segment : string,
    loginDetails : ILoginDetails,
    registerDetails: IRegisterDetails,
    webAuthSupported : boolean
}

export const LoginStateDefault : ILoginState = {
    segment: "login",
    loginDetails : {
        username: "", 
        password: "",
        foundCredential: false
    },
    registerDetails: {
        username: "", 
        password: "", 
        confirmPassword: "", 
        validUsername: false, 
        usernameError: "", 
        passwordInvalid: false, 
        confirmPasswordMatches:false,
        passwordless:false,
        buttonDisabled: false
    },
    webAuthSupported:false
}