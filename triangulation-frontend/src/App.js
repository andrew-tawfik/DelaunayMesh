import React, { useState, useEffect } from "react";
import Canvas from "./Canvas";
import "./App.css";

const App = () => {
    const [jsonData, setJsonData] = useState(null);
    const [dynamicPoints, setDynamicPoints] = useState([]);

    const handleCanvasClick = (x, y) => {
        console.log(`Point added: (${x}, ${y})`);
        setDynamicPoints((prevPoints) => [...prevPoints, { x, y }]);
    };

    useEffect(() => {
        fetch("/test.json") // Ensure this matches the file location in the public directory
            .then((response) => {
                if (!response.ok) {
                    throw new Error(`HTTP error! status: ${response.status}`);
                }
                return response.json();
            })
            .then((data) => setJsonData(data))
            .catch((error) => console.error("Error loading JSON:", error));
    }, []);
    

    return (
        <div className="App">
            <header className="App-header">
                {jsonData ? (
                    <Canvas data={jsonData} dynamicPoints={dynamicPoints} onCanvasClick={handleCanvasClick} />
                ) : (
                    <p>Loading...</p>
                )}
            </header>
        </div>
    );
};

export default App;
