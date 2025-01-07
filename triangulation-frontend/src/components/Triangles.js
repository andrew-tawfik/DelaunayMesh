import React from "react";
import * as d3 from "d3";

const Triangles = ({ triangles }) => {
    const renderTriangles = (svg, triangles) => {
        // Clear existing triangles
        svg.selectAll("path.triangle").remove();

        // Draw triangles
        svg.selectAll("path.triangle")
            .data(triangles)
            .enter()
            .append("path")
            .attr("class", "triangle")
            .attr("d", (triangle) => `
                M ${triangle.pt0.x} ${triangle.pt0.y}
                L ${triangle.pt1.x} ${triangle.pt1.y}
                L ${triangle.pt2.x} ${triangle.pt2.y}
                Z
            `)
            .attr("fill", "#1e1e1e")
            .attr("stroke", "silver")
            .attr("stroke-width", 2);
    };

    return (
        <g ref={(g) => renderTriangles(d3.select(g), triangles)} />
    );
};

export default Triangles;
