import {Form, Icon, Input, Button, Checkbox} from 'antd';
import {withRouter, Redirect} from "react-router";
import React from 'react';
import {Link} from "react-router-dom";
import axios from 'axios';
import {Alert} from 'antd';
import ReactDOM from 'react-dom';


class Add_device extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            login: false
        }
    }

    handleSubmit = e => {
        const _this = this;
        e.preventDefault();
        this.props.form.validateFields((err, values) => {
            if (!err) {
                var data={"Action":"insert","Token":localStorage.getItem('Token'),"Data":[values]};
                data.Data[0].DeviceID=Number(data.Data[0].DeviceID);
                var data2={"Action":"update","Token":localStorage.getItem('Token'),"Data":[{"DeviceID":Number(window.device_id), "DeviceName":values.DeviceName}]};
                console.log(data);
                if(window.device_id===''||window.device_id===undefined){
                axios.post("/api/device", JSON.stringify(data),{headers: {'Content-Type': 'application/json'}}).then(function (response) {
                    if (response.status===200) {
                        alert("添加成功");
                        _this.props.history.push({
                            pathname: "/device"
                        });
                    } else {
                        alert("添加失败");
                        _this.props.history.push({
                            pathname: "/device"
                        });
                    }
                })}else {
                    axios.post("/api/device", JSON.stringify(data2),{headers: {'Content-Type': 'application/json'}}).then(function (response) {
                        if (response.status===200) {
                            alert("修改成功");
                            _this.props.history.push({
                                pathname: "/device"
                            });
                            window.device_name="";
                            window.device_id="";
                        } else {
                            _this.props.history.push({
                                pathname: "/device"
                            });
                            console.log(-1);
                            window.device_name="";
                            window.device_id="";
                            alert("修改失败");
                        }})
                }
            }
        });
    };

    render() {
        const {getFieldDecorator} = this.props.form;
        return (
            <div>
            {window.device_id===''||window.device_id===undefined?<div>
            <h1 style={{
                display: "flex",
                justifyContent: "center",
                position: "relative",
                top: "20vh"
            }}>添加一个智能设备
            </h1>
            {this.state.login ? <Redirect to={{
                    pathname: "/device", query: {
                        token: this.state.login.Token,
                        id: window.college?window.college.sid:undefined,
                        school: window.college?window.college.name:undefined,
                        reload:true
                    }
                }}/> :
                <div/>}
            {/*<Link to={{pathname: '/'}}>*/}
            <Button type="default" shape="circle" icon="arrow-left" size="large"
                    style={{position: "relative", left: "2vw", top: "1vh"}}
                    onClick={
                        () => {
                            if (this.props.location.state) {
                                this.props.history.push({
                                    pathname: "/device",
                                    query: {data: this.props.location.state}
                                });
                            } else {
                                this.props.history.push({pathname: "/device"});
                            }
                        }
                    }>
            </Button>
            {/*</Link>*/}
            <Form onSubmit={this.handleSubmit} className="login-form" style={{
                maxWidth: "80vw",
                position: "relative",
                left: "10vw",
                right: "10vw",
                top: "30vh"
            }}>
                <Form.Item>
                    {getFieldDecorator('DeviceID', {
                        rules: [{required: true, message: 'Please input Device ID!'}],
                    })(
                        <Input
                            prefix={<Icon type="DeviceID" style={{color: 'rgba(0,0,0,.25)'}}/>}
                            placeholder="DeviceID"
                        />,
                    )}
                </Form.Item>
                <Form.Item>
                    {getFieldDecorator('DeviceName', {
                        rules: [{required: true, message: 'Please input device name!'}],
                    })(
                        <Input
                            prefix={<Icon type="DeviceName" style={{color: 'rgba(0,0,0,.25)'}}/>}
                            placeholder="DeviceName"
                        />,
                    )}
                </Form.Item>

                <div id="alert"/>
                <Form.Item>

                    <Button type="primary" htmlType="submit" className="login-form-button" style={{width: "50%",position:"relative",left:'20vw'}}>
                        Submit
                    </Button>

                </Form.Item>
            </Form>
        </div>:<div>
                <h1 style={{
                    display: "flex",
                    justifyContent: "center",
                    position: "relative",
                    top: "20vh"
                }}>{window.device_name}
                </h1>
                <h1 style={{
                    display: "flex",
                    justifyContent: "center",
                    position: "relative",
                    top: "20vh"
                }}>修改设备名称
                </h1>
                <div/>
                {this.state.login ? <Redirect to={{
                        pathname: "/device", query: {
                            token: this.state.login.Token,
                            id: window.college?window.college.sid:undefined,
                            school: window.college?window.college.name:undefined,
                            reload:true
                        }
                    }}/> :
                    <div/>}
                {/*<Link to={{pathname: '/'}}>*/}
                <Button type="default" shape="circle" icon="arrow-left" size="large"
                        style={{position: "relative", left: "2vw", top: "1vh"}}
                        onClick={
                            () => {
                                if (this.props.location.state) {
                                    this.props.history.push({
                                        pathname: "/device",
                                        query: {data: this.props.location.state}
                                    });
                                    window.device_name="";
                                    window.device_id="";
                                } else {
                                    this.props.history.push({pathname: "/device"});
                                    window.device_name="";
                                    window.device_id="";
                                }
                            }
                        }>
                </Button>
                {/*</Link>*/}
                <Form onSubmit={this.handleSubmit} className="login-form" style={{
                    maxWidth: "80vw",
                    position: "relative",
                    left: "10vw",
                    right: "10vw",
                    top: "30vh"
                }}>

                    <Form.Item>
                        {getFieldDecorator('DeviceName', {
                            rules: [{required: true, message: 'Please input device name!'}],
                        })(
                            <Input
                                prefix={<Icon type="DeviceName" style={{color: 'rgba(0,0,0,.25)'}}/>}
                                placeholder="DeviceName"
                            />,
                        )}
                    </Form.Item>

                    <div id="alert"/>
                    <Form.Item>

                        <Button type="primary" htmlType="submit" className="login-form-button" style={{width: "50%",position:"relative",left:'20vw'}}>
                            Submit
                        </Button>

                    </Form.Item>
                </Form>
            </div>}
            </div>
        );
    }
}


const WrappedAddDevice = Form.create({name: 'add_device'})(Add_device);
export default withRouter(WrappedAddDevice);