import React, { useEffect, useRef } from "react";
import * as d3 from "d3";


const Canvas = ({ data, dynamicPoints, onCanvasClick }) => {
    const canvasRef = useRef();

    useEffect(() => {
        const canvas = canvasRef.current;
        const svg = d3.select(canvas);

        // Clear the canvas
        svg.selectAll("*").remove();

        // Draw triangles from JSON
        data.triangles.forEach((triangle) => {
            const path = `
                M ${triangle.pt0.x} ${triangle.pt0.y}
                L ${triangle.pt1.x} ${triangle.pt1.y}
                L ${triangle.pt2.x} ${triangle.pt2.y}
                Z
            `;

            svg.append("path")
                .attr("d", path)
                .attr("fill", "#1e1e1e")
                .attr("stroke", "silver")
                .attr("stroke-width", 2);
        });

        // Draw static points from JSON
        data.points.forEach((point) => {
            svg.append("circle")
                .attr("cx", point.x)
                .attr("cy", point.y)
                .attr("r", 3)
                .attr("fill", "gold");
        });

        // Draw dynamic points
        dynamicPoints.forEach((point) => {
            svg.append("circle")
                .attr("cx", point.x)
                .attr("cy", point.y)
                .attr("r", 3)
                .attr("fill", "gold");
        });
    }, [data, dynamicPoints]);

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
        ></svg>
    );
};

export default Canvas;
