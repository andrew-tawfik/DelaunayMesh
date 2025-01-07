import React from "react";
import * as d3 from "d3";

const Points = ({ points }) => {
    const renderPoints = (svg, points) => {
        // Clear existing points
        svg.selectAll("circle.point").remove();

        // Draw points
        svg.selectAll("circle.point")
            .data(points)
            .enter()
            .append("circle")
            .attr("class", "point")
            .attr("cx", (point) => point.x)
            .attr("cy", (point) => point.y)
            .attr("r", 3)
            .attr("fill", "gold");
    };

    return (
        <g ref={(g) => renderPoints(d3.select(g), points)} />
    );
};

export default Points;
