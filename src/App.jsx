import {
  createBrowserRouter,
  RouterProvider,
} from "react-router-dom";

import { SettingsTab } from './tabs/Settings';
import { HistoryTab } from './tabs/History';

import Container from 'react-bootstrap/Container';
import Nav from 'react-bootstrap/Nav';
import Navbar from 'react-bootstrap/Navbar';

const router = createBrowserRouter([
  { path: "/settings?", element: <SettingsTab />},
  { path: "/history", element: <HistoryTab />},
]);

import "./App.css";

function App() {
  return (<>
    <Navbar className="lav-nav">
        <Nav className="me-auto pt-0  pb-0 p-2">
          <Nav.Link href="/history">History</Nav.Link>
          <Nav.Link href="/extension">Extensions</Nav.Link>
          <Nav.Link active href="/settings">Settings</Nav.Link>
          <Nav.Link href="/help">Help</Nav.Link>
        </Nav>
    </Navbar>
    <Container fluid className="row flex-fill flex-column">
      <RouterProvider router={router} />
    </Container>
  </>);
}

export default App;
