import {
    Form,
    Input,
    Tooltip,
    Icon,
    Cascader,
    Select,
    Row,
    Col,
    Checkbox,
    Button,
    AutoComplete, Alert,
} from 'antd';
import React from 'react';
import { Link, withRouter } from 'react-router-dom';
import axios from 'axios';
import ReactDOM from "react-dom";


const { Option } = Select;
const AutoCompleteOption = AutoComplete.Option;


class RegistrationForm extends React.Component {
    state = {
        confirmDirty: false,
        autoCompleteResult: [],
    };

    handleSubmit = e => {
        const _this = this;
        e.preventDefault();
        this.props.form.validateFieldsAndScroll((err, values) => {
            if (!err) {
                // console.log('Received values of form: ', values);
                axios.post("/api/register", {
                    UserName: values.nickname,
                    Password: values.password,
                }, { headers: { 'Content-Type': 'application/json' } }).then(response => {
                    if (response.status === 200) {
                        _this.props.history.push({ pathname: "/login" });
                    }
                }).catch(err => {
                    if (err.response) {
                        // console.log(err.response);
                        if (err.response.data.msg === "username used") {
                            ReactDOM.render(<Alert type="error"
                                message="Username used" />, document.getElementById("alert"))
                        }
                        if (err.response.data.msg === "you bad guy") {
                            ReactDOM.render(<Alert type="error"
                                message="Wrong code" />, document.getElementById("alert"))

                        } else {
                            console.log(err);
                        }
                    }
                })

            }
        });
    };

    handleConfirmBlur = e => {
        const { value } = e.target;
        this.setState({ confirmDirty: this.state.confirmDirty || !!value });
    };

    compareToFirstPassword = (rule, value, callback) => {
        const { form } = this.props;
        if (value && value !== form.getFieldValue('password')) {
            callback('Two passwords that you enter is inconsistent!');
        } else {
            callback();
        }
    };

    validateToNextPassword = (rule, value, callback) => {
        const { form } = this.props;
        if (value && this.state.confirmDirty) {
            form.validateFields(['confirm'], { force: true });
        }
        callback();
    };

    handleWebsiteChange = value => {
        let autoCompleteResult;
        if (!value) {
            autoCompleteResult = [];
        } else {
            autoCompleteResult = ['.com', '.org', '.net'].map(domain => `${value}${domain}`);
        }
        this.setState({ autoCompleteResult });
    };

    render() {
        const { getFieldDecorator } = this.props.form;
        const { autoCompleteResult } = this.state;

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
        const tailFormItemLayout = {
            wrapperCol: {
                xs: {
                    span: 24,
                    offset: 0,
                },
                sm: {
                    span: 16,
                    offset: 8,
                },
            },
        };
        const prefixSelector = getFieldDecorator('prefix', {
            initialValue: '86',
        })(
            <Select style={{ width: 70 }}>
                <Option value="86">+86</Option>
                <Option value="87">+87</Option>
            </Select>,
        );

        const websiteOptions = autoCompleteResult.map(website => (
            <AutoCompleteOption key={website}>{website}</AutoCompleteOption>
        ));

        return (
            <div>
                <Link to={{ pathname: '/' }}>
                    <Button type="default" shape="circle" icon="arrow-left" size="large"
                        style={{ position: "relative", left: "2vw", top: "1vh" }}>
                    </Button>
                </Link>
                <Form {...formItemLayout} onSubmit={this.handleSubmit}
                    style={{ position: "relative", top: "5vh", left: "10vw", right: "10vw", maxWidth: "80vw" }}>
                    <Form.Item
                        label={
                            <span>
                                Username&nbsp;
            </span>
                        }
                    >
                        {getFieldDecorator('nickname', {
                            rules: [{ required: true, message: 'Please input your nickname!', whitespace: true }],
                        })(<Input />)}
                    </Form.Item>
                    <Form.Item label="Password" hasFeedback>
                        {getFieldDecorator('password', {
                            rules: [
                                {
                                    required: true,
                                    message: 'Please input your password!',
                                },
                                {
                                    validator: this.validateToNextPassword,
                                },
                            ],
                        })(<Input.Password />)}
                    </Form.Item>
                    <Form.Item label="Confirm Password" hasFeedback>
                        {getFieldDecorator('confirm', {
                            rules: [
                                {
                                    required: true,
                                    message: 'Please confirm your password!',
                                },
                                {
                                    validator: this.compareToFirstPassword,
                                },
                            ],
                        })(<Input.Password onBlur={this.handleConfirmBlur} />)}
                    </Form.Item>

                    {/*<Form.Item label="Phone Number">*/}
                    {/*    {getFieldDecorator('phone', {*/}
                    {/*        rules: [{ required: true, message: 'Please input your phone number!' }],*/}
                    {/*    })(<Input addonBefore={prefixSelector} style={{ width: '100%' }} />)}*/}
                    {/*</Form.Item>*/}
                    {/*<Form.Item label="Website">*/}
                    {/*    {getFieldDecorator('website', {*/}
                    {/*        rules: [{ required: true, message: 'Please input website!' }],*/}
                    {/*    })(*/}
                    {/*        <AutoComplete*/}
                    {/*            dataSource={websiteOptions}*/}
                    {/*            onChange={this.handleWebsiteChange}*/}
                    {/*            placeholder="website"*/}
                    {/*        >*/}
                    {/*            <Input />*/}
                    {/*        </AutoComplete>,*/}
                    {/*    )}*/}
                    {/*</Form.Item>*/}
                    <Form.Item {...tailFormItemLayout}>
                        <Button type="primary" htmlType="submit">
                            Register
                        </Button>
                    </Form.Item>

                </Form>
                <div id="alert" style={{ position: "relative", top: "2.5vh" }} />
            </div>
        );
    }
}

const WrappedRegistrationForm = Form.create({ name: 'register' })(RegistrationForm);

export default withRouter(WrappedRegistrationForm);
