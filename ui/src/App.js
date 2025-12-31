import React from "react";
import Canvas from "./components/Canvas";
import useWebSocket from "./hooks/useWebSocket";
import "./App.css";

const App = () => {
    const { data, sendMessage } = useWebSocket("ws://localhost:9002");

    const handleCanvasClick = (x, y) => {
        console.log(`Point added: (${x}, ${y})`);
        sendMessage({
            action: "add_point",
            data: { x, y },
        });
    };

    return (
        <div className="App">
            <header className="App-header">
                <Canvas data={data} onCanvasClick={handleCanvasClick} />
            </header>
        </div>
    );
};

export default App;
