import React, { useEffect, useRef } from "react";
import * as d3 from "d3";

const Canvas = ({ points = [], onClick }) => {
    const canvasRef = useRef();

    useEffect(() => {
        // Select the canvas and set up D3 context
        const canvas = canvasRef.current;
        const svg = d3.select(canvas);

        // Clear any existing points
        svg.selectAll("circle").remove();

        // Draw points
        svg.selectAll("circle")
            .data(points)
            .enter()
            .append("circle")
            .attr("cx", (d) => d.x)
            .attr("cy", (d) => d.y)
            .attr("r", 4)
            .attr("fill", "gold");
    }, [points]); // Only re-run when points change

    return (
        <svg
            ref={canvasRef}
            width={1400}
            height={800}
            style={{ border: "1px solid black", margin: "20px auto", display: "block" }}
            onClick={onClick}
        ></svg>
    );
};

export default Canvas;