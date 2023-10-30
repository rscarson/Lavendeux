import React, { useEffect, useState } from "react";
import { invoke } from "@tauri-apps/api/primitives";
import { getCurrent } from "@tauri-apps/api/window";

interface Props {
    path: string,
    onLoad?: (s:string) => string
}

export function translate(path: string): Promise<string> {
    return new Promise((resolve, reject) => {
        invoke("translate", {path}).then(s => {
            resolve(s as string ?? path);
        }).catch(e => reject(e));
    })
}

export const Translated = (props: Props) => {
    let [value, setValue] = useState<string>();

    function updateTranslation() {
        translate(props.path).then(s => {
            let str = s as string;
            console.log("Translating: "+str);
            if (!str) return;
            if (props.onLoad) str = props.onLoad(str);
            setValue(str);
        })
    }

    useEffect(() => {
        updateTranslation();

        const appWindow = getCurrent();
        appWindow.listen("updated-language", () => updateTranslation())
    }, []);

    return <>{value}</>
}