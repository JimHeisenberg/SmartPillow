import {Form, Icon, Input, Button, Checkbox} from 'antd';
import {withRouter, Redirect} from "react-router";
import React from 'react';
import {Link} from "react-router-dom";
import axios from 'axios';
import {Alert} from 'antd';
import ReactDOM from 'react-dom';


class NormalLoginForm extends React.Component {
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
                axios.post("/api/login", JSON.stringify(values),{headers: {'Content-Type': 'application/json'}}).then(function (response) {
                    if (response.data.Token) {
                        localStorage.Username=values.UserName;
                        _this.setState({login: response.data});
                        localStorage.setItem("Token",response.data.Token);
                    } else {
                        _this.setState({login: false});
                    }
                }).catch(error => {
                    if(error.response)
                    if (error.response.status === 401) {
                        ReactDOM.render(<Alert type="error"
                                               message="Wrong username or password"/>, document.getElementById("alert"))
                    }
                })


            }
        });
    };

    render() {
        const {getFieldDecorator} = this.props.form;
        return (

            <div>
                {this.state.login ? <Redirect to={{
                        pathname: "/", query: {
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
                                        pathname: "/read",
                                        query: {data: this.props.location.state}
                                    });
                                } else {
                                    this.props.history.push({pathname: "/"});
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
                        {getFieldDecorator('UserName', {
                            rules: [{required: true, message: 'Please input your username!'}],
                        })(
                            <Input
                                prefix={<Icon type="user" style={{color: 'rgba(0,0,0,.25)'}}/>}
                                placeholder="Username"
                            />,
                        )}
                    </Form.Item>
                    <Form.Item>
                        {getFieldDecorator('Password', {
                            rules: [{required: true, message: 'Please input your Password!'}],
                        })(
                            <Input
                                prefix={<Icon type="lock" style={{color: 'rgba(0,0,0,.25)'}}/>}
                                type="password"
                                placeholder="Password"
                            />,
                        )}
                    </Form.Item>
                    <div id="alert"/>
                    <Form.Item>
                        {/*{getFieldDecorator('remember', {*/}
                        {/*    valuePropName: 'checked',*/}
                        {/*    initialValue: true,*/}
                        {/*})(*/}
                        <Checkbox>Remember me</Checkbox>
                        {/*)}*/}
                        <Button type="primary" htmlType="submit" className="login-form-button" style={{width: "100%"}}>
                            Log in
                        </Button>
                        Or <a href="/#/register">register now!</a>
                    </Form.Item>
                </Form>
            </div>
        );
    }
}


const WrappedNormalLoginForm = Form.create({name: 'normal_login'})(NormalLoginForm);
export default withRouter(WrappedNormalLoginForm);