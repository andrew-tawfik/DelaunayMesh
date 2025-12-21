import React, { useRef } from "react";
import Triangles from "./Triangles";
import Points from "./Points";

const Canvas = ({ data = { points: [], triangles: [] }, onCanvasClick }) => {
    const canvasRef = useRef();

    const handleClick = (event) => {
        const rect = canvasRef.current.getBoundingClientRect();
        const x = event.clientX - rect.left;
        const y = event.clientY - rect.top;
        onCanvasClick(x, y);
    };

    return (
        <svg
            ref={canvasRef}
            width={1400}
            height={800}
            style={{ border: "1px solid black", margin: "20px auto", display: "block" }}
            onClick={handleClick}
        > 
            <Triangles triangles={data.triangles.filter((triangle) => triangle.helper === false)} />
            <Points points={data.points} />
        </svg>
    );
};

export default Canvas;
