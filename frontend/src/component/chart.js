import { Button } from 'antd';
import axios from 'axios';
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

const timeFormat = (date)=>{
    const dayMap = ['日', '一', '二', '三', '四', '五', '六']
    const temp = new Date(date)
    return temp.getFullYear()+'年'+(temp.getMonth()+1)+'月'+temp.getDate()+'日'
    +' 星期'+dayMap[temp.getDay()]
}

const Chart2 = React.FC = ({data}) => {
    var ava = 0;
    var arr = [];
    var best_score = 0;
    var best_score_date = "";
    data.forEach(i => {
        if (i.type == '睡眠时间') {
            arr.push(i.value);
            if (i.value > best_score) {

                best_score_date = i.date
            }
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
        <h3 style={{ position: "relative", top: "5vh", margin: "30px" }}>您本周的平均睡眠时间:{(ava / 60).toFixed(2)}分钟</h3>
        <h3 style={{ position: "relative", top: "5vh", margin: "30px" }}>您本周睡眠质量最好的一天:{timeFormat(best_score_date)}</h3>
    </div>;
};
class Chart extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            data: []
        }
    }
    componentWillMount() {
        const _this = this;
        axios.post('api/chart', { "Action": "select", "Token": localStorage.getItem('Token') }, { headers: { 'Content-Type': 'application/json' } }).then(function (response) {
            if (response.data.Data) {
                _this.setState({ data: response.data.Data });
            } else {
                console.log('fetching chart data failed');
            }
        }).catch(error => {
            if (error.response)
                if (error.response.status === 401) {
                    console.log(401)
                }
        });
    }

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
                <Chart2 data={this.state.data} />
            </div>);
    }
}
export default withRouter(Chart);