import React from 'react';
import {Button, Col, Icon, Row, Input} from "antd";
import {Link} from 'react-router-dom';
import {withRouter} from 'react-router-dom';
import axios from 'axios';
import aa from './aa.jpg';
import bb from './bb.jpg';

const {Search} = Input;

class HomeSearch extends React.Component {
    constructor(props) {
        super(props);
        this.myInput = React.createRef();
        this.state = {
            login: false
        }
    }
    focusTextInput() {
        // console.log(this.myInput);
        this.myInput.current.focus();//获取焦点
    }

    componentWillMount() {
        if(this.props.location.query){
        if(this.props.location.query.reload){
            window.location.reload();
        }}
    }

    render() {
        let school, login;
        if (localStorage.getItem("Token")) {
            school = (
                <div>
                <Row className="search-bar-row">

                    <Col style={{display: "flex", justifyContent: "center",height:"50%"}}>
                        <Button type="primary" size="large">
                            <Link to="/device">
                                <p style={{textAlign: 'center', fontSize: '1.2em', position: 'relative', top: 5}}>
                                    Device Table</p>
                            </Link>
                        </Button>

                    </Col>
                    <br/>
                        <Col style={{display: "flex", justifyContent: "center",height:"50%"}}>
                            <Button type="primary" size="large">
                                <Link to="/setting">
                                    <p style={{textAlign: 'center', fontSize: '1.2em', position: 'relative', top: 5}}>
                                        Alarm Table</p>
                        </Link>
                            </Button>
                        </Col>

                    <br/>
                    <Col style={{display: "flex", justifyContent: "center",height:"50%"}}>
                        <Button type="primary" size="large">
                            <Link to="/chart">
                                <p style={{textAlign: 'center', fontSize: '1.2em', position: 'relative', top: 5}}>
                                    Statistics</p>
                            </Link>
                        </Button>

                    </Col>

                    <br/><br/><br/><br/><br/><br/><br/>
                        <Col style={{display: "flex", justifyContent: "center",height:"50%"}}>
                            <Button type="danger" size="default"  ghost onClick={function () {
                                localStorage.setItem('Token',"");
                                window.location.reload();
                            }}>
                                    <p style={{textAlign: 'center', fontSize: '1em'}}>
                                        Log Out</p>
                            </Button>
                        </Col>
                    </Row>
                </div>
            );
        }
        else{school=(<br/>)};
        if (localStorage.getItem("Token")) {
            login = <div><h1 style={{
                display: "flex",
                justifyContent: "center",
                position: "relative",
                top: "3vh"
            }}>Smart Pillow
            </h1>
                <h2 style={{
                display: "flex",
                justifyContent: "center",
                position: "relative",
                top: "10vh"
            }}>Welcome,{localStorage.getItem('Username')}
            </h2>
            </div>
        } else {
            login = (<div>
                <h1 style={{
                    display: "flex",
                    justifyContent: "center",
                    position: "relative",
                    top: "3vh"
                }}>Smart Pillow
                </h1>
                <div style={{textAlign:"center",position:"relative",top:50}}>
                {/*<img src='https://ss1.bdstatic.com/70cFuXSh_Q1YnxGkpoWK1HF6hhy/it/u=2370081220,2159129501&fm=26&gp=0.jpg' width={150} />*/}
                    <img src={aa} width={200}/>
                    <br/><br/>
                    <img src={bb} width={200}/>
                </div>
                <Row className="home-page-button" style={{display: "flex", justifyContent: "space-around",position:"relative",top:"15vh"}}>
                <Link to="/login">
                    <Button type="primary" ghost>
                        Sign In
                    </Button>
                </Link>
                <Link to="/register">
                    <Button>Sign Up</Button>
                </Link>
            </Row>
                <div style={{position:'relative',top:'22vh',margin:20}}>智能枕芯是一个通过物联网技术，与其它智能家居互联互通的嵌入式设备。旨在利用最小的成本，最大化地提升用户生活水平与生活便利程度。</div>
            </div>)
        }
        return (
            <div>
                {login}

                {school}
            </div>
        )
    }
}

export default withRouter(HomeSearch);
