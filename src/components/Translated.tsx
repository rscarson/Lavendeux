import React, { useEffect, useState } from "react";
import { invoke } from "@tauri-apps/api/core";
import { getCurrent } from "@tauri-apps/api/window";

interface Props {
    path: string,
    onLoad?: (s:string) => string
}

function fallback(s) {
    console.log(s);
    let _s = s.split('\\');
    console.log(_s[_s.length ? _s.length - 1 : 0]);
    return _s[_s.length ? _s.length - 1 : 0];
}

export function translate(path: string): Promise<string> {
    return TranslationCache.fetch(path);
}

class TranslationCache {
    static async fetch(path: string): Promise<string> {
        const cachedValue = localStorage.getItem(path);
        if (cachedValue === null || cachedValue == "null") {
            const s: string = await invoke("translate", {path});
            localStorage.setItem(path, s);
            return s;
        } else {
            return cachedValue;
        }
    }
}

export function clearTranslationCache() {
    localStorage.clear();
}

export const Translated = (props: Props) => {
    let [value, setValue] = useState<string>(localStorage.getItem(props.path)??props.path);

    function updateTranslation() {
        translate(props.path).then(s => {
            let str = s as string;
            if (!str) return;
            if (props.onLoad) str = props.onLoad(str);
            setValue(str);
        })
    }

    useEffect(() => {
        updateTranslation();

        const appWindow = getCurrent();
        appWindow.listen("language-updated", () => {
            updateTranslation()
        })
    }, []);

    return <>{value}</>
}