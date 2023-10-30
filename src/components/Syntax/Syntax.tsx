import React from "react";

import { LavendeuxFormatter } from './formatter';

import Spinner from 'react-bootstrap/Spinner';
import Container from "react-bootstrap/Container";

import './formatter/formatter.css';

interface Props {
    code: string
    className?: string
}

export const Syntax = (props: Props) => {
    const formatter = new LavendeuxFormatter();
    return (
        <pre 
            className={`formatted-sample ${props.className && ''}`} 
        >
            <code spellcheck="false" contentEditable="plaintext-only" dangerouslySetInnerHTML={{__html:formatter.format(props.code)}}>
            </code>
        </pre>
    );
}