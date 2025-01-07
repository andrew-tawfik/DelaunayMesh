import { useEffect, useRef, useState, useCallback } from "react";

const useWebSocket = (url, delay = 50) => {
    const [data, setData] = useState({ points: [], triangles: [] });
    const ws = useRef(null);

    const sendMessage = useCallback((message) => {
        if (ws.current && ws.current.readyState === WebSocket.OPEN) {
            ws.current.send(JSON.stringify(message));
        } else {
            console.error("Unable to send message, WebSocket connection is not open.");
        }
    }, []);

    useEffect(() => {
        const connectWebSocket = () => {
            ws.current = new WebSocket(url);

            ws.current.onopen = () => {
                console.log("WebSocket connection established");
                sendMessage({ action: "fetch_mesh" });
            };

            ws.current.onmessage = (event) => {
                try {
                    const serverMessage = JSON.parse(event.data);
                    console.log("Message from server:", serverMessage);

                    if (serverMessage.points || serverMessage.triangles) {
                        setData(serverMessage);
                    } else {
                        console.warn("Server response does not contain valid mesh data");
                    }
                } catch (error) {
                    console.error("Error parsing server message:", error);
                }
            };

            ws.current.onerror = () => {
                console.error("WebSocket connection error");
            };

            ws.current.onclose = () => {
                console.log("WebSocket connection closed");
            };
        };

        const timeoutId = setTimeout(connectWebSocket, delay);

        return () => {
            clearTimeout(timeoutId);
            if (ws.current) {
                ws.current.close();
            }
        };
    }, [url, delay, sendMessage]);

    return { data, sendMessage };
};

export default useWebSocket;
