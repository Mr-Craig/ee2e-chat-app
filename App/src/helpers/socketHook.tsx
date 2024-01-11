import {useState, useEffect, useCallback} from 'react';
import Socket from './socket';

export function useSocket(event : string, callback : (data: any) => void, deps: React.DependencyList) {
    const socketCallback = useCallback(callback, deps);
    const [eventIndex, setEventIndex] = useState<number>();
    useEffect(() => {
        Socket.clearEvent(event, eventIndex);
        let i = Socket.on(event, socketCallback);
        setEventIndex(i);
    }, [socketCallback]);
}