import { Button } from 'antd';
import ReactDOM from 'react-dom';
import { withRouter, Redirect } from "react-router";
import React, { useState, useEffect } from 'react';
import { Line } from '@ant-design/charts';
function getAve(array) {
    var ave = 0;
    for (var i = 0; i < array.length; i++) {
        ave += array[i];
    }
    ave /= i;
    ave = ave.toFixed(1);
    return ave;
}

const Chart2 = React.FC = () => {
    const data = [
        {
            date: '6/1',
            type: '睡眠时间',
            value: 6.4,
        },
        {
            date: '6/1',
            type: '翻身次数',
            value: 11,
        },
        {
            date: '6/2',
            type: '睡眠时间',
            value: 8,
        },
        {
            date: '6/2',
            type: '翻身次数',
            value: 14,
        },
        {
            date: '6/3',
            type: '睡眠时间',
            value: 7.6,
        },
        {
            date: '6/3',
            type: '翻身次数',
            value: 11,
        },
        {
            date: '6/4',
            type: '睡眠时间',
            value: 8.7,
        },
        {
            date: '6/4',
            type: '翻身次数',
            value: 9,
        },
        {
            date: '6/5',
            type: '睡眠时间',
            value: 5.9,
        },
        {
            date: '6/5',
            type: '翻身次数',
            value: 11,
        },
        {
            date: '6/6',
            type: '睡眠时间',
            value: 5.6,
        },
        {
            date: '6/6',
            type: '翻身次数',
            value: 15,
        },
        {
            date: '6/7',
            type: '睡眠时间',
            value: 9.3,
        },
        {
            date: '6/7',
            type: '翻身次数',
            value: 5,
        },
    ];

    var ava = 0;
    var arr = [];
    var best_date = "6/7";
    data.forEach(i => {
        if (i.type == '睡眠时间') {
            arr.push(i.value);
        }
    });
    ava = getAve(arr);
    const config = {
        title: {
            visible: true,
            text: '近一周睡眠质量统计图表',
        },
        description: {
            visible: true,
            text: '智能枕芯将会统计您的睡眠时间和翻身次数:',
        },
        padding: 'auto',
        forceFit: true,
        data,
        xField: 'date',
        yField: 'value',
        yAxis: { label: { formatter: (v) => `${v}`.replace(/\d{1,3}(?=(\d{3})+$)/g, (s) => `${s},`) } },
        legend: { position: 'right-top' },
        seriesField: 'type',
        color: ['#1979C9', '#D62A0D', '#FAA219'],
        responsive: true,
        smooth: true,
    };
    return <div>
        <div style={{ position: "relative", top: "5vh" }}><Line {...config} /></div>
        <div style={{ position: "relative", top: "5vh", margin: "30px" }}>人只有在浅睡眠的时候才会翻身,翻身次数和睡眠时间可以大致反应一个人的睡眠质量如何</div>
        <h3 style={{ position: "relative", top: "5vh", margin: "30px" }}>您本周的平均睡眠时间:{ava}</h3>
        <h3 style={{ position: "relative", top: "5vh", margin: "30px" }}>您本周睡眠质量最好的一天:{best_date}</h3>
    </div>;
};
class Chart extends React.Component {
    render() {
        return (
            <div>
                <Button type="default" shape="circle" icon="arrow-left" size="large"
                    style={{ position: "relative", left: "2vw", top: "1vh" }}
                    onClick={
                        () => {
                            this.props.history.push({
                                pathname: "/"
                            });
                        }
                    }>
                </Button>
                <Chart2 />
            </div>);
    }
}
export default withRouter(Chart);