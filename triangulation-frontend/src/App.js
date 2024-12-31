import React, { useState } from "react";
import Canvas from "./Canvas";
import "./App.css";

const App = () => {
    const [points, setPoints] = useState([]);

    const handleCanvasClick = (e) => {
        const canvas = e.target;
        const rect = canvas.getBoundingClientRect();

        // Get click position relative to canvas
        const x = e.clientX - rect.left;
        const y = e.clientY - rect.top;

        console.log(`Point added: (${x}, ${y})`);

        // Add the new point to the state
        setPoints((prevPoints) => [...prevPoints, { x, y }]);
    };

    return (
        <div className="App">
            <header className="App-header">
                <Canvas points={points} onClick={handleCanvasClick} />
            </header>
        </div>
    );
};

export default App;
