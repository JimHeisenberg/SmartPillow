import { List, Avatar, Icon, Alert, Button } from 'antd';
import axios from 'axios';
import ReactDOM from 'react-dom';
import React from 'react';
import { withRouter } from "react-router";
import { Link } from "react-router-dom";




// function _delete(id){
//     console.log(toString(id));
// }
//
// function _update(id) {
//     console.log(toString(id));
// }

const IconText = ({ type, text }) => (
    <span>
        <Icon type={type} style={{ marginRight: 8 }} />
        {text}
    </span>
);


class Setting extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            data: false
        }
    }
    componentWillMount() {
        const _this = this;
        window.listData = [];
        axios.post('/api/setting', { "Action": "select", "Token": localStorage.getItem('Token') }, { headers: { 'Content-Type': 'application/json' } }).then(function (response) {
            if (response.data.Data) {
                console.log(response.data);

                for (let i = 0; i < response.data.Data.length; i++) {
                    // console.log(response.data.Data[i]);
                    window.listData.push({
                        description: response.data.Data[i].TimeID,
                        avatar: 'https://zos.alipayobjects.com/rmsportal/ODTLcjxAfvqbxHnVXCYX.png',
                        title:
                            "SleepingTime:" + response.data.Data[i].SleepingTime + " " + " " + "WakeupTime:" + response.data.Data[i].WakeupTime,
                        content:
                            '',
                    });
                    _this.setState({ data: true });
                }
            } else {
                console.log('failed');
            }
        }).catch(error => {
            if (error.response)
                if (error.response.status === 401) {
                    ReactDOM.render(<Alert type="error"
                        message="Wrong username or password" />, document.getElementById("alert"))
                }
        });
        console.log(window.listData[0]);
    }

    render() {
        setTimeout(1000);
        return (<div><Button type="default" shape="circle" icon="arrow-left" size="large"
            style={{ position: "relative", left: "2vw", top: "1vh" }}
            onClick={
                () => {
                    if (this.props.location.state) {
                        this.props.history.push({
                            pathname: "/",
                            query: { data: this.props.location.state }
                        });
                    } else {
                        this.props.history.push({ pathname: "/" });
                    }
                }
            }>
        </Button>
            <h2 style={{
                display: "flex",
                justifyContent: "center",
                position: "relative",
                top: "3vh"
            }}>闹钟列表
            </h2>
            <List style={{ position: "relative", left: "3vw", width: "94%", top: "5vh" }}
                itemLayout="vertical"
                size="large"
                pagination={{
                    onChange: page => {
                        console.log(page);

                    },
                    pageSize: 3,
                }}
                dataSource={window.listData}
                renderItem={item => (
                    <List.Item
                        key={item.title}
                        actions={[
                            <Link to={{ pathname: "/set_time" }}>
                                <Icon type="setting" onClick={() => { window.TimeID = item.description }} /></Link>,
                            <Icon type="close-circle" onClick={() => {
                                axios.post('/api/setting', { "Action": "delete", "Token": localStorage.getItem('Token'), "Data": [{ "TimeID": item.description, "SleepingTime": "13:30:00", "WakeupTime": "14:00:00" }] }, { headers: { 'Content-Type': 'application/json' } }).then(function (response) {
                                    if (response.data.Token) {
                                        console.log(2);
                                        window.location.reload();
                                    } else {
                                        console.log(1);
                                        window.location.reload();
                                    }
                                })
                            }} />,

                        ]}
                        extra={
                            <img
                                width={100}
                                alt="logo"
                                src="https://timgsa.baidu.com/timg?image&quality=80&size=b9999_10000&sec=1590066046183&di=91b3b7d5351648faf0133d2a21897f49&imgtype=0&src=http%3A%2F%2Fdpic.tiankong.com%2Fuz%2Fkh%2FQJ6266602917.jpg"
                            />
                        }
                    >
                        <List.Item.Meta
                            avatar={<Avatar src={item.avatar} />}
                            title={item.title}
                            description={item.description}
                        />
                        {item.content}
                    </List.Item>
                )}
            /><Link to={{ pathname: "/set_time" }}>
                <Button type="primary" shape="circle" icon="plus" size="large" style={{
                    width: "7vh",
                    height: "7vh",
                    boxShadow: "0vh 0.7vh 1vh #aaaaaa",
                    position: "fixed",
                    bottom: "3vh",
                    right: "3vh",
                    zIndex: 1
                }} />
            </Link></div>);
    }

}


export default withRouter(Setting);