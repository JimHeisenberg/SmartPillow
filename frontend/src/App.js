import React from 'react';
import logo from './logo.svg';
import './App.css';
import { Route, Switch } from "react-router-dom";

import HomeSearch from "./component/home-search";

import WrappedNormalLoginForm from "./component/login"
import WrappedRegistrationForm from "./component/register"

import Device from "./component/device";
import WrappedAddDevice from "./component/add_device";
import Setting from "./component/setting";
import WrappedSetTime from "./component/set_time"
import Chart from "./component/chart";

class App extends React.Component {
    render() {
        return (
            <div>
                <Route exact={true} path="/" component={HomeSearch} />
                <Route path="/login" component={WrappedNormalLoginForm} />
                <Route path="/register" component={WrappedRegistrationForm} />
                <Route path="/device" component={Device} />
                <Route path="/add_device" component={WrappedAddDevice} />
                <Route path="/setting" component={Setting} />
                <Route path="/set_time" component={WrappedSetTime} />
                <Route path="/chart" component={Chart} />
            </div>
        )
    }
}
export default App;
