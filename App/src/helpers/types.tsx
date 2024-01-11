export enum EMessageState
{
    /* read and delivered */
    READ,
    /* got a deliver receipt but its not been read yet */
    DELIVERED,
    /* sent but not receieved (user offline etc.) */
    SENT
}

export enum EMessageSender {
    RECEIVER,
    SENDER
}

export interface IMessage
{
    id: string,
    contents: string,
    time: number,
    state : EMessageState
    who : EMessageSender
}

export enum EChatState
{
    READY,
    KEYEXCHANGE
}

export interface IChat
{
    username : string,
    state : EChatState,
    ecKey? : { 
        publicKey: string,
        privateKey: string
    },
    sharedSecret? : string,
    messages : { [id : string] : IMessage}
}

export enum EContactState
{
    ACCEPTED,
    PENDING,
    DENIED,
    SENT,
    /* special case where the server rejects the contact request, only due to the username not existing. */
    REJECTED
}
export interface IContact
{
    username : string,
    contactState : EContactState
}

export interface IUserData
{
    chats : { [username : string] : IChat },
    contacts : { [username : string] : IContact },
    notifications : boolean
}

export interface ISettings
{
    userData : { [username : string] : IUserData },
    lastUsername : string,
    credentials : { [username : string] : string }
}