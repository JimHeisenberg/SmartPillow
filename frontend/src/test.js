import React from 'react';
import ReactDOM from 'react-dom';
import {Button} from 'antd';
import * as serviceWorker from './serviceWorker';


ReactDOM.render(
    <div>
        <Button type="primary">Primary</Button>
        <Button>Default</Button>
        <Button type="dashed">Dashed</Button>
        <Button type="danger">Danger</Button>
        <Button type="link">Link</Button>
    </div>,
    document.getElementById('root')
);

serviceWorker.unregister();
