import React from 'react';
import { withRouter, Link, Redirect, Router } from 'react-router-dom';
import { Form, DatePicker, TimePicker, Button } from 'antd';
import { Checkbox } from 'antd';
import axios from 'axios';
import ReactDOM from "react-dom";

const { MonthPicker, RangePicker } = DatePicker;
class SetTime extends React.Component {

    handleSubmit = e => {
        e.preventDefault();

        this.props.form.validateFields((err, fieldsValue) => {
            if (err) {
                return;
            }

            // Should format date value before submit.
            const rangeValue = fieldsValue['range-picker'];
            const rangeTimeValue = fieldsValue['range-time-picker'];
            const values = {
                ...fieldsValue,
                // 'date-picker': fieldsValue['date-picker'].format('YYYY-MM-DD'),
                // 'date-time-picker': fieldsValue['date-time-picker'].format('YYYY-MM-DD HH:mm:ss'),
                // 'month-picker': fieldsValue['month-picker'].format('YYYY-MM'),
                // 'range-picker': [rangeValue[0].format('YYYY-MM-DD'), rangeValue[1].format('YYYY-MM-DD')],
                // 'range-time-picker': [
                //     rangeTimeValue[0].format('YYYY-MM-DD HH:mm:ss'),
                //     rangeTimeValue[1].format('YYYY-MM-DD HH:mm:ss'),
                // ],
                'SleepingTime': fieldsValue['SleepingTime'].format('HH:mm:ss'),
                'WakeupTime': fieldsValue['WakeupTime'].format('HH:mm:ss'),
            };
            const _this = this;
            var data = { "Action": "insert", "Token": localStorage.getItem('Token'), "Data": [values] };
            var values2 = values;
            values2.TimeID = window.TimeID;
            var data2 = { "Action": "update", "Token": localStorage.getItem('Token'), "Data": [values2] };
            if (window.TimeID === '' || window.TimeID === undefined) {
                // console.log(123);
                axios.post("/api/setting", JSON.stringify(data), { headers: { 'Content-Type': 'application/json' } }).then(function (response) {
                    if (response) {
                        _this.props.history.push({
                            pathname: "/setting"
                        });
                    } else {
                        console.log(-1);
                    }
                })
            } else {
                axios.post("/api/setting", JSON.stringify(data2), { headers: { 'Content-Type': 'application/json' } }).then(function (response) {
                    if (response) {
                        window.TimeID = '';
                        _this.props.history.push({
                            pathname: "/setting"
                        });
                    } else {
                        window.TimeID = '';
                        console.log(-1);
                    }
                })
            }

        });
    };
    render() {
        const { getFieldDecorator } = this.props.form;
        const formItemLayout = {
            labelCol: {
                xs: { span: 24 },
                sm: { span: 8 },
            },
            wrapperCol: {
                xs: { span: 24 },
                sm: { span: 16 },
            },
        };
        const config = {
            rules: [{ type: 'object', required: true, message: 'Please select time!' }],
        };
        const rangeConfig = {
            rules: [{ type: 'array', required: true, message: 'Please select time!' }],
        };
        return (
            <div><Button type="default" shape="circle" icon="arrow-left" size="large"
                style={{ position: "relative", left: "2vw", top: "1vh" }}
                onClick={
                    () => {
                        if (this.props.location.state) {
                            this.props.history.push({
                                pathname: "/setting"
                            });
                        } else {
                            this.props.history.push({ pathname: "/setting" });
                        }
                    }
                }>
            </Button>
                <h2 style={{
                    display: "flex",
                    justifyContent: "center",
                    position: "relative",
                    top: "18vh"
                }}>设置一个闹钟
                </h2>
                <Form {...formItemLayout} onSubmit={this.handleSubmit} style={{ position: "relative", left: '30vw', top: '20vh', width: "70%", height: "50%" }}>
                    {/*<Form.Item label="DatePicker">*/}
                    {/*    {getFieldDecorator('date-picker', config)(<DatePicker />)}*/}
                    {/*</Form.Item>*/}
                    {/*<Form.Item label="DatePicker[showTime]">*/}
                    {/*    {getFieldDecorator('date-time-picker', config)(*/}
                    {/*        <DatePicker showTime format="YYYY-MM-DD HH:mm:ss" />,*/}
                    {/*    )}*/}
                    {/*</Form.Item>*/}
                    {/*<Form.Item label="MonthPicker">*/}
                    {/*    {getFieldDecorator('month-picker', config)(<MonthPicker />)}*/}
                    {/*</Form.Item>*/}
                    {/*<Form.Item label="RangePicker">*/}
                    {/*    {getFieldDecorator('range-picker', rangeConfig)(<RangePicker />)}*/}
                    {/*</Form.Item>*/}
                    {/*<Form.Item label="RangePicker[showTime]">*/}
                    {/*    {getFieldDecorator('range-time-picker', rangeConfig)(*/}
                    {/*        <RangePicker showTime format="YYYY-MM-DD HH:mm:ss" />,*/}
                    {/*    )}*/}
                    {/*</Form.Item>*/}
                    <Form.Item label="SleepingTime">
                        {getFieldDecorator('SleepingTime', config)(<TimePicker />)}
                    </Form.Item>
                    <Form.Item label="WakeupTime">
                        {getFieldDecorator('WakeupTime', config)(<TimePicker />)}
                    </Form.Item>
                    <Form.Item
                        wrapperCol={{
                            xs: { span: 24, offset: 0 },
                            sm: { span: 16, offset: 8 },
                        }}
                    >
                        <Button type="primary" htmlType="submit" style={{ width: "31%" }}>
                            Submit
                    </Button>
                    </Form.Item>
                </Form>
            </div>
        );
    }
}
const WrappedSetTime = Form.create({ name: 'time_related_controls' })(SetTime);

export default withRouter(WrappedSetTime);